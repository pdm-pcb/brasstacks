#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/imgui/ImGuiContext.hpp"

#include "brasstacks/platform/imgui/FiraMono_data.hpp"

#include "brasstacks/platform/vulkan/vkInstance.hpp"
#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"
#include "brasstacks/platform/vulkan/passes/vkColorDepthPass.hpp"

namespace btx {

::ImGuiIO *ImGuiContext::_io { nullptr };
::ImGuiStyle *ImGuiContext::_style { nullptr };

std::unique_ptr<vkDescriptorPool> ImGuiContext::_descriptor_pool {
    std::make_unique<vkDescriptorPool>()
};

bool ImGuiContext::_enabled { true };

// =============================================================================
void ImGuiContext::init_window(::GLFWwindow *window) {
    ::ImGui::CreateContext();
    ::ImGui_ImplGlfw_InitForVulkan(window, true);
    ::ImGui::StyleColorsDark();

    _io = &::ImGui::GetIO();
    _io->IniFilename = nullptr;
    _io->LogFilename = nullptr;

    _io->Fonts->AddFontFromMemoryCompressedTTF(
        &FiraMono_compressed_data,
        FiraMono_compressed_size,
        16.0f
    );

    _style = &::ImGui::GetStyle();
    _style->ScaleAllSizes(TargetWindow::scale_factor());

    _style->Alpha = 0.75f;
    _style->DisabledAlpha = 0.33f;
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
        // _draw_perf_window();

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
        if(::ImGui::BeginMenu("Application")) {
            if(::ImGui::MenuItem("Exit", "Esc")) {
                EventBus::publish(WindowEvent(WindowEventType::WINDOW_CLOSE));
            }

            ::ImGui::EndMenu();
        }
        if(::ImGui::BeginMenu("Settings")) {
            if(::ImGui::BeginMenu("Resolution")) {
                for(auto &res : RenderConfig::resolutions) {
                    static std::string res_name;
                    res_name = fmt::format("{}x{}", res.size.width,
                                                    res.size.height);

                    if(::ImGui::MenuItem(res_name.c_str(), "", &res.selected)) {
                        TargetWindow::size_and_place(res.size);
                        RenderConfig::current_resolution = &res;
                        for(auto &other_res : RenderConfig::resolutions) {
                            if(&res != &other_res) {
                                other_res.selected = false;
                            }
                        }
                    }
                }

                ::ImGui::EndMenu();
            }

            ::ImGui::EndMenu();
        }
        ::ImGui::EndMainMenuBar();
    }
}

// =============================================================================
void ImGuiContext::_draw_status_bar() {
    auto const *viewport = ::ImGui::GetMainViewport();
    auto const y_offset = _style->FramePadding.y * 2.0f + ImGui::GetFontSize();

    ::ImVec2 const pos(viewport->Pos.x,
                       viewport->Pos.y + (viewport->Size.y - y_offset));

    ::ImVec2 const size(viewport->Size.x, 1.0f);

    ::ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
    ::ImGui::SetNextWindowSize(size, ImGuiCond_Always);

    ::ImGui::Begin("StatusBar", nullptr, (ImGuiWindowFlags_NoDecoration
                                          | ImGuiWindowFlags_NoMove
                                          | ImGuiWindowFlags_NoNav
                                          | ImGuiWindowFlags_MenuBar));
    ::ImGui::BeginMenuBar();
    ::ImGui::BeginTable("StatusBarTable", 2);
        ::ImGui::TableNextRow();
        ::ImGui::TableSetColumnIndex(0);
        ::ImGui::Text("CPU Time: %.06f",
                      static_cast<double>(TimeKeeper::delta_time()));
        ::ImGui::TableNextColumn();
        ::ImGui::Text("Resolution: %ux%u",
                      RenderConfig::current_resolution->size.width,
                      RenderConfig::current_resolution->size.height);
    ::ImGui::EndTable();
    ::ImGui::EndMenuBar();
    ::ImGui::End();
}

} // namespace btx