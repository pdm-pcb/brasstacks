#include "brasstacks/core.hpp"
#include "brasstacks/system/DebugOverlay.hpp"

#include "brasstacks/system/TargetWindow.hpp"

#include "brasstacks/platform/vulkan/vkInstance.hpp"
#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/rendering/vkSwapchain.hpp"
#include "brasstacks/platform/vulkan/rendering/vkDebugOverlayPass.hpp"
#include "brasstacks/platform/vulkan/devices/vkQueue.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdBuffer.hpp"
#include "brasstacks/platform/vulkan/descriptors/vkDescriptorPool.hpp"
#include "brasstacks/platform/vulkan/rendering/vkFramebuffer.hpp"
#include "brasstacks/platform/vulkan/resources/vkImageView.hpp"

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_vulkan.h>

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
DebugOverlay::DebugOverlay(vkDevice const &device, TargetWindow const &target_window,
                 vkSwapchain const &swapchain) :
    _device          { device },
    _descriptor_pool { nullptr },
    _framebuffers    { },
    _overlay_pass    { nullptr }
{
    _allocate_descriptor_pool();

    _overlay_pass = new vkDebugOverlayPass(_device, swapchain.image_format());

    _create_framebuffers(swapchain);
    _init_imgui(target_window, swapchain);
}

// =============================================================================
DebugOverlay::~DebugOverlay() {
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
void DebugOverlay::render_ui(vkCmdBuffer const &cmd_buffer,
                             uint32_t const image_index)
{
    ::ImGui_ImplWin32_NewFrame();
    ::ImGui_ImplVulkan_NewFrame();
    ImGui::NewFrame();
    ImGui::ShowDemoWindow();
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
void DebugOverlay::_allocate_descriptor_pool() {
    _descriptor_pool = new vkDescriptorPool(
        _device,
        vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
        1000u,
        {
            { vk::DescriptorType::eSampler,              1000u, },
            { vk::DescriptorType::eCombinedImageSampler, 1000u, },
            { vk::DescriptorType::eSampledImage,         1000u, },
            { vk::DescriptorType::eStorageImage,         1000u, },
            { vk::DescriptorType::eUniformTexelBuffer,   1000u, },
            { vk::DescriptorType::eStorageTexelBuffer,   1000u, },
            { vk::DescriptorType::eUniformBuffer,        1000u, },
            { vk::DescriptorType::eStorageBuffer,        1000u, },
            { vk::DescriptorType::eUniformBufferDynamic, 1000u, },
            { vk::DescriptorType::eStorageBufferDynamic, 1000u, },
            { vk::DescriptorType::eInputAttachment,      1000u, },
            { vk::DescriptorType::eInlineUniformBlock,   1000u, },
        }
    );
}

// =============================================================================
void DebugOverlay::_create_framebuffers(vkSwapchain const &swapchain) {
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
void DebugOverlay::_init_imgui(TargetWindow const &target_window,
                          vkSwapchain const &swapchain)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();

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
        .MinImageCount = static_cast<uint32_t>(swapchain.images().size()),
        .ImageCount = static_cast<uint32_t>(swapchain.images().size()),
        .MSAASamples = VK_SAMPLE_COUNT_1_BIT,
        .UseDynamicRendering = false,
        .ColorAttachmentFormat = VK_FORMAT_UNDEFINED,
        .Allocator = nullptr,
        .CheckVkResultFn = check_result,
    };

    ::ImGui_ImplVulkan_Init(&init_info, _overlay_pass->native());

    ::ImGui_ImplVulkan_CreateFontsTexture();
}

} // namespace btx