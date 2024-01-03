#include "brasstacks/core.hpp"
#include "brasstacks/system/UILayer.hpp"

#include "brasstacks/system/TargetWindow.hpp"
#include "brasstacks/platform/win32/Win32TargetWindow.hpp"

#include "brasstacks/platform/vulkan/vkInstance.hpp"
#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/rendering/vkSwapchain.hpp"
#include "brasstacks/platform/vulkan/rendering/vkUILayerPass.hpp"
#include "brasstacks/platform/vulkan/devices/vkQueue.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdBuffer.hpp"
#include "brasstacks/platform/vulkan/descriptors/vkDescriptorPool.hpp"
#include "brasstacks/platform/vulkan/rendering/vkFramebuffer.hpp"
#include "brasstacks/platform/vulkan/resources/vkImageView.hpp"

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_vulkan.h>

#include "brasstacks/assets/fonts/hack_regular.hpp"

// Very temporary... promise. =)
#include "brasstacks/events/EventBroker.hpp"
#include "brasstacks/events/keyboard_events.hpp"

namespace btx {

// =============================================================================
static void check_result(VkResult err) {
    if(err == VK_SUCCESS) {
        return;
    }

    if(err < VK_SUCCESS) {
        BTX_CRITICAL("Dear ImGui returned: '{}'",
                     vk::to_string(vk::Result(err)));
        return;
    }

    BTX_WARN("Dear ImGui returned: '{}'", vk::to_string(vk::Result(err)));
}

// =============================================================================
UILayer::UILayer(vkDevice const &device, TargetWindow &target_window,
                 vkSwapchain const &swapchain) :
    _device          { device },
    _target_window   { target_window },
    _descriptor_pool { nullptr },
    _framebuffers    { },
    _overlay_pass    { nullptr }
{
    _allocate_descriptor_pool();

    _overlay_pass = new vkUILayerPass(_device, swapchain.image_format());

    _create_framebuffers(swapchain);
    _init_imgui(target_window);

    _target_window.set_ui_input(true);
}

// =============================================================================
UILayer::~UILayer() {
    _target_window.set_ui_input(false);

    ::ImGui_ImplVulkan_DestroyFontsTexture();
    ::ImGui_ImplVulkan_Shutdown();
    ::ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    for(auto *framebuffer : _framebuffers) {
        delete framebuffer;
    }

    delete _overlay_pass;
    delete _descriptor_pool;
}

// =============================================================================
void UILayer::render_pass(vkCmdBuffer const &cmd_buffer,
                          uint32_t const image_index)
{
    ::ImGui_ImplWin32_NewFrame();
    ::ImGui_ImplVulkan_NewFrame();

    ImGui::NewFrame();

        ImGui::ShowDemoWindow();
        // _draw_title_bar();
        // _draw_menu();

    ImGui::Render();

    vk::Rect2D const render_area = {
        .offset {
            .x = RenderConfig::swapchain_image_offset.x,
            .y = RenderConfig::swapchain_image_offset.y,
        },
        .extent {
            .width  = RenderConfig::swapchain_image_size.width,
            .height = RenderConfig::swapchain_image_size.height,
        }
    };

    auto const &framebuffer = *_framebuffers[image_index];
    cmd_buffer.begin_render_pass(
        vk::RenderPassBeginInfo {
            .pNext           = nullptr,
            .renderPass      = _overlay_pass->native(),
            .framebuffer     = framebuffer.native(),
            .renderArea      = render_area,
            .clearValueCount = 0u,
            .pClearValues    = nullptr,
        }
    );

    ::ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(),
                                      cmd_buffer.native());

    cmd_buffer.end_render_pass();
}

// =============================================================================
void UILayer::_allocate_descriptor_pool() {
    _descriptor_pool = new vkDescriptorPool(
        _device,
        vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
        1,
        {
            { vk::DescriptorType::eCombinedImageSampler, 1u, },
        }
    );
}

// =============================================================================
void UILayer::_create_framebuffers(vkSwapchain const &swapchain) {
    for(size_t i = 0; i < RenderConfig::swapchain_image_count; ++i) {
        _framebuffers.emplace_back(new vkFramebuffer(
            _device,
            *_overlay_pass,
            {
                .width  = RenderConfig::swapchain_image_size.width,
                .height = RenderConfig::swapchain_image_size.height,
            },
            {{
                swapchain.image_views()[i]->native()
            }}
        ));
    }
}

// =============================================================================
void UILayer::_init_imgui(TargetWindow const &target_window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    auto &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    io.Fonts->AddFontFromMemoryCompressedTTF(
        hack_regular_compressed_data,
        hack_regular_compressed_size,
        14.0f * target_window.dpi_scale()
    );

    auto &style = ImGui::GetStyle();
    style.ScaleAllSizes(target_window.dpi_scale());

    ::ImGui_ImplWin32_Init(target_window.native());

    ::ImGui_ImplVulkan_InitInfo init_info = {
        .Instance = vkInstance::native(),
        .PhysicalDevice = vkPhysicalDevice::native(),
        .Device = _device.native(),
        .QueueFamily = _device.graphics_queue().family_index(),
        .Queue = _device.graphics_queue().native(),
        .PipelineCache = VK_NULL_HANDLE,
        .DescriptorPool = _descriptor_pool->native(),
        .Subpass = 0u,
        .MinImageCount = RenderConfig::swapchain_image_count,
        .ImageCount = RenderConfig::swapchain_image_count,
        .MSAASamples = VK_SAMPLE_COUNT_1_BIT,
        .UseDynamicRendering = false,
        .ColorAttachmentFormat = VK_FORMAT_UNDEFINED,
        .Allocator = nullptr,
        .CheckVkResultFn = check_result,
    };

    ::ImGui_ImplVulkan_Init(&init_info, _overlay_pass->native());

    ::ImGui_ImplVulkan_CreateFontsTexture();
}

// =============================================================================
void UILayer::_draw_title_bar() {
    static float const titlebar_height = 32.0f;

    static ImGuiWindowFlags const titlebar_flags =
        ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoScrollbar
        | ImGuiWindowFlags_NoSavedSettings
        | ImGuiWindowFlags_NoScrollWithMouse;

    auto const *viewport = ImGui::GetMainViewport();

    auto &io = ImGui::GetIO();

    // check if mouse cursor is inside the titlebar area & compensate
    // 7px padding: is used for the window border resize
    if((io.MousePos.x >= 7 && io.MousePos.x <= (viewport->WorkSize.x - 7))
       && (io.MousePos.y >= 7 && io.MousePos.y <= titlebar_height))
    {
        // titlebar_area = true;

        // if(io.MouseDownDuration[5] == 0.000000f) {
        //     // started dragging
        //     dragging_window = true;
        //     init_mouse_x = io.MousePos.x;
        //     init_mouse_y = io.MousePos.y;
        // }

        // if(io.MouseDownDuration[5] == -1.000000f) {
        //     // stopped dragging
        //     dragging_window = false;
        // }
    }
    else {
        // titlebar_area = false;
    }

    // if(dragging_window) {
    //     temp_window_x += io.MousePos.x - init_mouse_x;
    //     temp_window_y += io.MousePos.y - init_mouse_y;
    //     ::SetWindowPos(hwnd, NULL, temp_window_x, temp_window_y,
    //                    viewport->WorkSize.x, viewport->WorkSize.y, NULL);
    // }

    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(ImVec2{ viewport->WorkSize.x, titlebar_height });

    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2{ 0.0f, 0.0f });

    // Properly center the content accoring to the titlebar height
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,
                        ImVec2{ titlebar_height / 3, titlebar_height / 3 });
    ImGui::PushStyleColor(ImGuiCol_WindowBg,
                          ImVec4{ 0.01f, 0.01f, 0.01f, 1.0f });

    ImGui::Begin("window-frame-titlebar", nullptr, titlebar_flags);
        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor(1);
        ImGui::SetWindowFontScale(1.0f);
        ImGui::TextColored(ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f },
                           "Application Title");
    ImGui::End();
}

// =============================================================================
void UILayer::_draw_menu() {
    if(ImGui::BeginMainMenuBar()) {
        if(ImGui::BeginMenu("File")) {
            if(ImGui::MenuItem("Exit")) {
                EventBroker::emit<KeyReleaseEvent>({ BTX_KB_ESCAPE });
            }
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Edit")) {
            if(ImGui::MenuItem("Undo", "CTRL+Z")) {}
            if(ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
            ImGui::Separator();
            if(ImGui::MenuItem("Cut", "CTRL+X")) {}
            if(ImGui::MenuItem("Copy", "CTRL+C")) {}
            if(ImGui::MenuItem("Paste", "CTRL+V")) {}
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

} // namespace btx