#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/ImGuiContext.hpp"

#include "brasstacks/platform/vulkan/vkInstance.hpp"
#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"
#include "brasstacks/platform/vulkan/passes/vkColorDepthPass.hpp"

namespace btx {

::ImGuiIO *ImGuiContext::_io { nullptr };
std::unique_ptr<vkDescriptorPool> ImGuiContext::_descriptor_pool {
    std::make_unique<vkDescriptorPool>()
};

// =============================================================================
void ImGuiContext::init_window(::GLFWwindow *window) {
	::ImGui::CreateContext();
	::ImGui_ImplGlfw_InitForVulkan(window, true);
    ::ImGui::StyleColorsDark();

    _io = &::ImGui::GetIO();
}

// =============================================================================
void ImGuiContext::create_descriptor_pool() {
    _descriptor_pool->create(
        vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
        1000u,
        {
            { vk::DescriptorType::eUniformBuffer,        1000u, },
            { vk::DescriptorType::eCombinedImageSampler, 1000u, },
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

} // namespace btx