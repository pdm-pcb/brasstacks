#ifndef BRASSTACKS_SYSTEM_GUIOVERLAY_HPP
#define BRASSTACKS_SYSTEM_GUIOVERLAY_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkInstance;
class vkPhysicalDevice;
class vkDevice;
class vkQueue;
class vkDescriptorPool;
class TargetWindow;
class vkColorDepthPass;

// class GUIOverlay final {
// public:
//     GUIOverlay(vkInstance const &instance,
//                vkPhysicalDevice const &physical_device, vkDevice const &device,
//                uint32_t const swapchain_image_count, TargetWindow const &window,
//                vkColorDepthPass const &render_pass);
//     ~GUIOverlay();

//     void draw_ui();

// private:
//     vkDescriptorPool *_descriptor_pool;
// };

} // namespace btx

#endif // BRASSTACKS_SYSTEM_GUIOVERLAY_HPP