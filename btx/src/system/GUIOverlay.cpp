#include "brasstacks/system/GUIOverlay.hpp"


#include "brasstacks/system/TargetWindow.hpp"
#include "brasstacks/platform/vulkan/rendering/vkRenderPass.hpp"
#include "brasstacks/platform/vulkan/vkInstance.hpp"
#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkQueue.hpp"
#include "brasstacks/platform/vulkan/descriptors/vkDescriptorPool.hpp"

void imgui_check_result(VkResult const result) {
    if(result != VK_SUCCESS) {
        BTX_CRITICAL("ImGUI got '{}'", vk::to_string(vk::Result(result)));
    }
}

namespace btx {

// =============================================================================
GUIOverlay::GUIOverlay(vkInstance const &instance,
                       vkPhysicalDevice const &physical_device,
                       vkDevice const &device,
                       vkDescriptorPool const &descriptor_pool,
                       uint32_t const swapchain_image_count,
                       TargetWindow const &window,
                       vkRenderPass const &render_pass)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ::ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ::ImGui_ImplWin32_Init(window.native());

    ImGui_ImplVulkan_InitInfo init_info = {
        .Instance = instance.native(),
        .PhysicalDevice = physical_device.native(),
        .Device = device.native(),
        .QueueFamily = physical_device.graphics_queue_index(),
        .Queue = device.graphics_queue().native(),
        .PipelineCache = { },
        .DescriptorPool = descriptor_pool.native(),
        .Subpass = { },
        .MinImageCount = swapchain_image_count,
        .ImageCount = swapchain_image_count,
        .MSAASamples = { },
        .CheckVkResultFn = imgui_check_result,
    };

    ::ImGui_ImplVulkan_Init(&init_info, render_pass.native());
}

// =============================================================================
GUIOverlay::~GUIOverlay() {
    ::ImGui_ImplVulkan_Shutdown();
    ::ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

// =============================================================================
void GUIOverlay::draw_ui() {
    ::ImGui_ImplVulkan_NewFrame();
    ::ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    static bool show_window = true;
    if(show_window) {
        ImGui::ShowDemoWindow(&show_window);
    }

    ImGui::Render();
}

} // namespace btx