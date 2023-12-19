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
class vkRenderPass;

class GUIOverlay final {
public:
    GUIOverlay(vkInstance const &instance, vkPhysicalDevice const &physical_device,
               vkDevice const &device, vkDescriptorPool const &descriptor_pool,
               uint32_t const swapchain_image_count, TargetWindow const &window,
               vkRenderPass const &render_pass);
    ~GUIOverlay();

    void draw_ui();
};

} // namespace btx

#endif // BRASSTACKS_SYSTEM_GUIOVERLAY_HPP