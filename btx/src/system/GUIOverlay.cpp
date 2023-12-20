#include "brasstacks/system/GUIOverlay.hpp"

#include "brasstacks/system/TargetWindow.hpp"
#include "brasstacks/platform/vulkan/rendering/vkRenderPass.hpp"
#include "brasstacks/platform/vulkan/vkInstance.hpp"
#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkQueue.hpp"
#include "brasstacks/platform/vulkan/descriptors/vkDescriptorPool.hpp"

namespace btx {

// // =============================================================================
// GUIOverlay::GUIOverlay(vkInstance const &instance,
//                        vkPhysicalDevice const &physical_device,
//                        vkDevice const &device,
//                        uint32_t const swapchain_image_count,
//                        TargetWindow const &window,
//                        vkRenderPass const &render_pass)
// {
//     _descriptor_pool = new vkDescriptorPool(
//         device,
//         1000u,
//         {
//             { vk::DescriptorType::eSampler,              1000u },
//             { vk::DescriptorType::eCombinedImageSampler, 1000u },
//             { vk::DescriptorType::eSampledImage,         1000u },
//             { vk::DescriptorType::eStorageImage,         1000u },
//             { vk::DescriptorType::eUniformTexelBuffer,   1000u },
//             { vk::DescriptorType::eStorageTexelBuffer,   1000u },
//             { vk::DescriptorType::eUniformBuffer,        1000u },
//             { vk::DescriptorType::eStorageBuffer,        1000u },
//             { vk::DescriptorType::eUniformBufferDynamic, 1000u },
//             { vk::DescriptorType::eStorageBufferDynamic, 1000u },
//             { vk::DescriptorType::eInputAttachment,      1000u },
//         }
//     );
// }

// // =============================================================================
// GUIOverlay::~GUIOverlay() {
//     delete _descriptor_pool;
// }

// // =============================================================================
// void GUIOverlay::draw_ui() {
// }

} // namespace btx