#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/imgui/ImGuiContext.hpp"

#include "brasstacks/platform/imgui/FiraMono_data.hpp"

#include "brasstacks/platform/vulkan/vkInstance.hpp"
#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"
#include "brasstacks/platform/vulkan/passes/vkColorDepthPass.hpp"

#include "brasstacks/events/menu_events.hpp"

namespace btx {

::ImGuiIO *ImGuiContext::_io { nullptr };
::ImGuiStyle *ImGuiContext::_style { nullptr };

std::string ImGuiContext::_window_title { };

std::unique_ptr<vkDescriptorPool> ImGuiContext::_descriptor_pool {
    std::make_unique<vkDescriptorPool>()
};

bool ImGuiContext::_enabled { true };

// =============================================================================
void ImGuiContext::init_window(::GLFWwindow *window,
                               std::string_view const window_title) {
    ::ImGui::CreateContext();
    ::ImGui_ImplGlfw_InitForVulkan(window, true);
    ::ImGui::StyleColorsDark();

    _io = &::ImGui::GetIO();
    _io->IniFilename = nullptr;
    _io->LogFilename = nullptr;

    _io->Fonts->AddFontFromMemoryCompressedTTF(
        &FiraMono_compressed_data,
        FiraMono_compressed_size,
        20.0f
    );

    _style = &::ImGui::GetStyle();
    _style->ScaleAllSizes(TargetWindow::scale_factor());

    _style->Alpha = 0.75f;
    _style->DisabledAlpha = 0.33f;

    _window_title = window_title;
}

// =============================================================================
void ImGuiContext::create_descriptor_pool() {
    _descriptor_pool->create(
        vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
        10u,
        {
            { vk::DescriptorType::eUniformBuffer,        10u, },
            { vk::DescriptorType::eCombinedImageSampler, 10u, },
        }
    );
}

// =============================================================================
void ImGuiContext::destroy_descriptor_pool() {
    _descriptor_pool->destroy();
}

// =============================================================================
void
ImGuiContext::create_swapchain_resources(vkColorDepthPass const &render_pass) {
    auto const image_count =
        static_cast<uint32_t>(Renderer::swapchain().images().size());

    ::ImGui_ImplVulkan_InitInfo init_info = {
        .Instance       = vkInstance::native(),
        .PhysicalDevice = vkPhysicalDevice::native(),
        .Device         = Renderer::device().native(),
        .QueueFamily    = vkPhysicalDevice::graphics_queue_index(),
        .Queue          = Renderer::device().graphics_queue().native(),
        .DescriptorPool = _descriptor_pool->native(),
        .RenderPass     = render_pass.native(),
        .MinImageCount  = image_count,
        .ImageCount     = image_count,
        .MSAASamples    = VkSampleCountFlagBits(render_pass.msaa_samples()),

        .PipelineCache = nullptr,
        .Subpass       = 0u,

        .UseDynamicRendering         = false,
        .PipelineRenderingCreateInfo = { },

        .Allocator = nullptr,
        .CheckVkResultFn = nullptr,
        .MinAllocationSize  = 1024*1024,
    };

    ::ImGui_ImplVulkan_Init(&init_info);
}

// =============================================================================
void ImGuiContext::destroy_swapchain_resources() {
    ::ImGui_ImplVulkan_Shutdown();
}

// =============================================================================
void ImGuiContext::shutdown_window() {
    ::ImGui_ImplGlfw_Shutdown();
    ::ImGui::DestroyContext();
}

// =============================================================================
void ImGuiContext::record_commands() {
    ::ImGui_ImplVulkan_NewFrame();
    ::ImGui_ImplGlfw_NewFrame();
    ::ImGui::NewFrame();
    ::ImGui::BeginDisabled(!_enabled);

        _draw_menu_bar();
        _draw_status_bar();
        // ::ImGui::ShowDemoWindow();

    ::ImGui::EndDisabled();
    ::ImGui::EndFrame();
}

// =============================================================================
void ImGuiContext::render(vkCmdBuffer const &cmd_buffer) {
    ::ImGui::Render();
    ::ImGui_ImplVulkan_RenderDrawData(::ImGui::GetDrawData(),
                                      cmd_buffer.native());
}

// =============================================================================
void ImGuiContext::_draw_menu_bar() {
    if(::ImGui::BeginMainMenuBar()) {
        if(::ImGui::BeginMenu(_window_title.c_str())) {
            if(::ImGui::MenuItem("Exit", "Esc")) {
                EventBus::publish(MenuEvent(MenuEventType::MENU_EXIT));
            }
            ::ImGui::EndMenu();
        }
        ::ImGui::Separator();
        ::ImGui::EndMainMenuBar();
    }
}

// =============================================================================
void ImGuiContext::_draw_status_bar() {
    auto const *viewport = ::ImGui::GetMainViewport();
    auto const y_offset = (_style->FramePadding.y * 2.0f)
                          + ::ImGui::GetFontSize();

    ::ImVec2 const pos(viewport->Pos.x,
                       viewport->Pos.y + (viewport->Size.y - y_offset));

    ::ImVec2 const size(viewport->Size.x, 1.0f);

    static std::string label_text;

    ::ImGui::SetNextWindowPos(pos, ::ImGuiCond_Always);
    ::ImGui::SetNextWindowSize(size, ::ImGuiCond_Always);

    ::ImGui::Begin("StatusBar", nullptr, (::ImGuiWindowFlags_NoDecoration
                                          | ::ImGuiWindowFlags_NoMove
                                          | ::ImGuiWindowFlags_NoNav
                                          | ::ImGuiWindowFlags_MenuBar));
    ::ImGui::BeginMenuBar();
    ::ImGui::BeginTable("StatusBarTable", 6);
        ::ImGui::TableNextRow();

        ::ImGui::TableSetColumnIndex(0);
        label_text = std::format("{}", vkPhysicalDevice::name());
        ::ImGui::Text("%s", label_text.c_str());

        ::ImGui::TableNextColumn();
        ::ImGui::Separator();
        auto &current_res = RenderConfig::current_resolution;
        label_text = std::format("{}x{}", current_res->size.width,
                                         current_res->size.height);
        if(::ImGui::BeginMenu(label_text.c_str())) {
            for(auto &res : RenderConfig::available_resolutions) {
                label_text = std::format("{}x{}", res.size.width,
                                                  res.size.height);

                if(::ImGui::MenuItem(label_text.c_str(), "", &res.selected)) {
                    current_res = &res;
                    for(auto &other_res : RenderConfig::available_resolutions) {
                        if(&res != &other_res) {
                            other_res.selected = false;
                        }
                    }
                    EventBus::publish(
                        MenuEvent(MenuEventType::MENU_CHANGE_WINDOW_SIZE)
                    );
                }
            }
            ::ImGui::EndMenu();
        }


        ::ImGui::TableNextColumn();
        ::ImGui::Separator();
        label_text = std::format("MSAA: x{}", RenderConfig::current_msaa);
        if(::ImGui::BeginMenu(label_text.c_str())) {
            for(auto const msaa : RenderConfig::available_msaa) {
                label_text = std::format("x{}", msaa);
                if(::ImGui::MenuItem(label_text.c_str())) {
                    RenderConfig::current_msaa = msaa;
                    // An event to recreate the render pass, knowing whether
                    // a resolve attachment is needed
                }
            }
            ::ImGui::EndMenu();
        }

        ::ImGui::TableNextColumn();
        ::ImGui::Separator();
        label_text = std::format("AF: {}x", RenderConfig::current_aniso);
        if(::ImGui::BeginMenu(label_text.c_str())) {
            for(auto const aniso : RenderConfig::available_aniso) {
                label_text = std::format("{}x", aniso);
                if(::ImGui::MenuItem(label_text.c_str())) {
                    RenderConfig::current_aniso = aniso;
                    // An event to recreate image samplers
                }
            }
            ::ImGui::EndMenu();
        }

        ::ImGui::TableNextColumn();
        ::ImGui::Separator();
        if(::ImGui::Checkbox("VSync", &RenderConfig::vsync_on)) {
            // An event to recreate the swapchain
        }

        ::ImGui::TableNextColumn();
        ::ImGui::Separator();
        label_text = std::format("CPU Time: {:.06f}", TimeKeeper::delta_time());
        ::ImGui::Text("%s", label_text.c_str());

    ::ImGui::EndTable();
    ::ImGui::EndMenuBar();
    ::ImGui::End();
}

} // namespace btx