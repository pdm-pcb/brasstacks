#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/imgui/ImGuiContext.hpp"

#include "brasstacks/platform/imgui/FiraMono_data.hpp"

#include "brasstacks/platform/vulkan/vkInstance.hpp"
#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"
#include "brasstacks/platform/vulkan/passes/vkColorDepthPass.hpp"

namespace btx {

::ImGuiIO *ImGuiContext::_io { nullptr };

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

    auto const font_size = std::floorf(16.0f * TargetWindow::scale_factor());

    _io->Fonts->AddFontFromMemoryCompressedTTF(
        &FiraMono_compressed_data,
        FiraMono_compressed_size,
        font_size
    );
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
        .PipelineCache  = nullptr,
        .DescriptorPool = _descriptor_pool->native(),
        .Subpass        = 0u,
        .MinImageCount  = image_count,
        .ImageCount     = image_count,
        .MSAASamples    = VkSampleCountFlagBits(render_pass.msaa_samples()),
    };

    ::ImGui_ImplVulkan_Init(&init_info, render_pass.native());
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
        _draw_perf_window();

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
            if(::ImGui::MenuItem("Exit")) {
                EventBus::publish(WindowEvent(WindowEventType::WINDOW_CLOSE));
            }
            ::ImGui::EndMenu();
        }
        ::ImGui::EndMainMenuBar();
    }
}

// =============================================================================
// struct TimeGraphState {
//     std::array<float, 64> points { };
//     const char* name = "Unassigned";
// };

// static void plotTimeGraph(float _newPoint, TimeGraphState& _rGraphState) {
//     std::rotate(_rGraphState.points.begin(),
//                 _rGraphState.points.begin() + 1,
//                 _rGraphState.points.end());

//     _rGraphState.points.back() = _newPoint;

//     static char title[64];
//     sprintf(title, "%s Time: %.2f ms", _rGraphState.name, _newPoint);

//     float kMinPlotValue = 0.0f;
//     float kMaxPlotValue = 20.0f;
//     ImGui::PlotLines(
//         "",
//         &_rGraphState.points[0],
//         static_cast<int>(_rGraphState.points.size()),
//         0,
//         title,
//         kMinPlotValue,
//         kMaxPlotValue,
//         ImVec2(300.0f, 50.0f)
//     );
// }

void ImGuiContext::_draw_perf_window() {
    // static bool bWindowHovered = false;

    // ImGui::SetNextWindowPos(ImVec2(25, 30), ImGuiCond_FirstUseEver);
    // ImGui::SetNextWindowBgAlpha(bWindowHovered ? 0.8f : 0.4f);

    // ImGui::Begin("Performance",
    //              nullptr,
    //              (ImGuiWindowFlags_AlwaysAutoResize
    //               | ImGuiWindowFlags_NoCollapse));

    // bWindowHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_None);

    // ImGui::Text("Device: %s", vkPhysicalDevice::name().data());
    // ImGui::Separator();

    // {
    //     static TimeGraphState timeGraph {
    //         .name = "Frame Time"
    //     };
    //     plotTimeGraph(1.e3f * TimeKeeper::delta_time(), timeGraph);
    // }

    // ImGui::End();
}

} // namespace btx