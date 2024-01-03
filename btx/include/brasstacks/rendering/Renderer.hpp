/**
 * @file Renderer.hpp
 * @brief The Vulkan rendering backend.
 */

#ifndef BRASSTACKS_RENDERING_RENDERER_HPP
#define BRASSTACKS_RENDERING_RENDERER_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class TargetWindow;
class UILayer;

class vkSurface;
class vkDevice;
class vkSwapchain;
class vkFrameSync;
class vkCmdBuffer;

/**
 * @brief The Vulkan rendering backend.
 *
 * Renderer manages everything Vulkan-specific.
 */
class Renderer final{
public:
    /**
     * @brief Construct the Renderer object.
     * @param target_window Used to create a rendering surface.
     */
    explicit Renderer(TargetWindow &target_window);

    ~Renderer();

    uint32_t acquire_next_image();
    vkCmdBuffer const & begin_recording();
    void end_recording();
    void submit_commands();
    void present_image();

    void wait_device_idle();

    inline auto const & device() const { return *_device; }
    inline auto const & swapchain() const { return *_swapchain; }

    Renderer() = delete;

    Renderer(Renderer &&) = delete;
    Renderer(const Renderer &) = delete;

    Renderer & operator=(Renderer &&) = delete;
    Renderer & operator=(const Renderer &) = delete;

private:
    vkDevice    *_device;
    vkSurface   *_surface;
    vkSwapchain *_swapchain;

    /**
     * @brief A queue of semaphores for acquiring images from the swapchain.
     *
     * At most, this queue will be of size 1 + the number of images available
     * from the swapchain. Each time a new image is requested, a semaphore is
     * removed from the queue, provided to the swapchain, and then stored in a
     * vkFrameSync object. If the vkFrameSync object being used already had an
     * image acquire semaphore, the old one is returned to this queue.
     */
    std::queue<vk::Semaphore> _image_acquire_sems;

    /**
     * @brief A collection of frame synchronization objects
     */
    std::vector<vkFrameSync *> _frame_sync;

    /**
     * @brief The index of the swapchain's next available image. This value is
     * also used to index Renderer's internal vkFrameSync objects.
     */
    uint32_t _image_index;

    UILayer *_ui_layer;

    /**
     * @brief Create the structures to synchronize commands with swapchain
     * images as well as the framebuffers
     */
    void _create_frame_sync();

    /**
     * @brief Called from the destructor
     */
    void _destroy_frame_sync();
};

} // namespace btx

#endif // BRASSTACKS_RENDERING_RENDERER_HPP