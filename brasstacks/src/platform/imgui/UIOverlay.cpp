#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/imgui/UIOverlay.hpp"

#include "brasstacks/platform/imgui/FiraMono_data.hpp"

#include "brasstacks/platform/vulkan/vkInstance.hpp"
#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"
#include "brasstacks/platform/vulkan/pipeline/vkPipeline.hpp"
#include "brasstacks/platform/vulkan/passes/vkColorDepthResolvePass.hpp"

#include "brasstacks/events/ui_events.hpp"

namespace btx {

::ImGuiIO *UIOverlay::_io { nullptr };
::ImGuiStyle *UIOverlay::_style { nullptr };

std::string UIOverlay::_window_title { };

std::unique_ptr<vkDescriptorPool> UIOverlay::_descriptor_pool {
    std::make_unique<vkDescriptorPool>()
};

bool UIOverlay::_enabled { true };

// =============================================================================
void UIOverlay::init_window(::GLFWwindow *window,
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
void UIOverlay::create_descriptor_pool() {
    _descriptor_pool->create(
        vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
        100u,
        {
            { vk::DescriptorType::eUniformBuffer,        100u, },
            { vk::DescriptorType::eCombinedImageSampler, 100u, },
        }
    );
}

// =============================================================================
void UIOverlay::destroy_descriptor_pool() {
    _descriptor_pool->destroy();
}

// =============================================================================
void
UIOverlay::create_swapchain_resources(vkRenderPassBase const &render_pass) {
    auto const image_count =
        static_cast<uint32_t>(Renderer::swapchain().images().size());

    auto const sample_flags =
        vkPipeline::samples_to_flag(RenderConfig::current_msaa->msaa);

    ::ImGui_ImplVulkan_InitInfo init_info = {
        .Instance       = vkInstance::native(),
        .PhysicalDevice = RenderConfig::current_device->handle,
        .Device         = Renderer::device().native(),
        .QueueFamily    = RenderConfig::current_device->graphics_queue_index,
        .Queue          = Renderer::device().graphics_queue().native(),
        .DescriptorPool = _descriptor_pool->native(),
        .RenderPass     = render_pass.native(),
        .MinImageCount  = image_count,
        .ImageCount     = image_count,
        .MSAASamples    = VkSampleCountFlagBits(sample_flags),

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
void UIOverlay::destroy_swapchain_resources() {
    ::ImGui_ImplVulkan_Shutdown();
}

// =============================================================================
void UIOverlay::shutdown_window() {
    ::ImGui_ImplGlfw_Shutdown();
    ::ImGui::DestroyContext();
}

// =============================================================================
void UIOverlay::record_commands() {
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
void UIOverlay::render() {
    ::ImGui::Render();
    ::ImGui_ImplVulkan_RenderDrawData(::ImGui::GetDrawData(),
                                      Renderer::cmd_buffer().native());
}

// =============================================================================
void UIOverlay::_draw_menu_bar() {
    if(::ImGui::BeginMainMenuBar()) {
        if(::ImGui::BeginMenu(_window_title.c_str())) {
            if(::ImGui::MenuItem("Exit", "Esc")) {
                EventBus::publish(UIEvent(UIEventType::UI_EXIT));
            }
            ::ImGui::EndMenu();
        }
        ::ImGui::Separator();
        ::ImGui::EndMainMenuBar();
    }
}

// =============================================================================
void UIOverlay::_draw_status_bar() {
    auto const *viewport = ::ImGui::GetMainViewport();
    auto const y = (_style->FramePadding.y * 2.0f)
                          + ::ImGui::GetFontSize();

    ::ImVec2 const pos(viewport->Pos.x,
                       viewport->Pos.y + (viewport->Size.y - y));

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
        label_text = std::format("{}", RenderConfig::current_device->name);
        if(::ImGui::BeginMenu(label_text.c_str())) {
            for(auto &device : RenderConfig::available_devices) {
                label_text = std::format("{}", device.name);
                if(::ImGui::MenuItem(label_text.c_str(), "", &device.selected))
                {
                    RenderConfig::current_device = &device;
                    for(auto &other_device : RenderConfig::available_devices) {
                        if(&device != &other_device) {
                            other_device.selected = false;
                        }
                    }
                    EventBus::publish(UIEvent(UIEventType::UI_CHANGE_DEVICE));
                }
            }
            ::ImGui::EndMenu();
        }

        ::ImGui::TableNextColumn();
        ::ImGui::Separator();
        label_text = std::format("{}x{}",
                                 RenderConfig::current_resolution->size.width,
                                 RenderConfig::current_resolution->size.height);
        if(::ImGui::BeginMenu(label_text.c_str())) {
            for(auto &res : RenderConfig::available_resolutions) {
                label_text = std::format("{}x{}", res.size.width,
                                                  res.size.height);

                if(::ImGui::MenuItem(label_text.c_str(), "", &res.selected)) {
                    RenderConfig::current_resolution = &res;
                    for(auto &other_res : RenderConfig::available_resolutions) {
                        if(&res != &other_res) {
                            other_res.selected = false;
                        }
                    }
                    EventBus::publish(UIEvent(UIEventType::UI_WINDOW_RESIZE));
                }
            }
            ::ImGui::EndMenu();
        }


        ::ImGui::TableNextColumn();
        ::ImGui::Separator();
        label_text = std::format("MSAA: x{}", RenderConfig::current_msaa->msaa);
        if(::ImGui::BeginMenu(label_text.c_str())) {
            for(auto &level : RenderConfig::available_msaa) {
                label_text = std::format("x{}", level.msaa);
                if(::ImGui::MenuItem(label_text.c_str(), "", &level.selected)) {
                    RenderConfig::current_msaa = &level;
                    for(auto &other_level : RenderConfig::available_msaa) {
                        if(&level != &other_level) {
                            other_level.selected = false;
                        }
                    }
                    EventBus::publish(UIEvent(UIEventType::UI_CHANGE_MSAA));
                }
            }
            ::ImGui::EndMenu();
        }

        ::ImGui::TableNextColumn();
        ::ImGui::Separator();
        label_text = std::format("AF: {}x", RenderConfig::current_aniso->aniso);
        if(::ImGui::BeginMenu(label_text.c_str())) {
            for(auto &level : RenderConfig::available_aniso) {
                label_text = std::format("{}x", level.aniso);
                if(::ImGui::MenuItem(label_text.c_str(), "", &level.selected)) {
                    RenderConfig::current_aniso = &level;
                    for(auto &other_level : RenderConfig::available_aniso) {
                        if(&level != &other_level) {
                            other_level.selected = false;
                        }
                    }
                    EventBus::publish(UIEvent(UIEventType::UI_CHANGE_ANISO));
                }
            }
            ::ImGui::EndMenu();
        }

        ::ImGui::TableNextColumn();
        ::ImGui::Separator();
        if(::ImGui::Checkbox("VSync", &RenderConfig::vsync_on)) {
            // An event to recreate the swapchain
            EventBus::publish(UIEvent(UIEventType::UI_TOGGLE_VSYNC));
        }

        ::ImGui::TableNextColumn();
        ::ImGui::Separator();
        label_text = std::format("CPU Time: {:.06f}ms",
                                 1e3f * TimeKeeper::delta_time());
        ::ImGui::Text("%s", label_text.c_str());

    ::ImGui::EndTable();
    ::ImGui::EndMenuBar();
    ::ImGui::End();
}

} // namespace btx