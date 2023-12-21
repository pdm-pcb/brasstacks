/**
 * @file Renderer.hpp
 * @brief The Vulkan rendering backend.
 */

#ifndef BRASSTACKS_RENDERING_RENDERER_HPP
#define BRASSTACKS_RENDERING_RENDERER_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class TargetWindow;

class vkInstance;
class vkPhysicalDevice;
class vkDevice;
class vkSurface;
class vkSwapchain;
class vkRenderPass;
class vkDescriptorPool;
class vkDescriptorSetLayout;
class vkDescriptorSet;
class vkPipeline;

class vkFrameSync;
class vkFramebuffer;

class vkBuffer;
class CubeMesh;
class FPSCamera;

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
    explicit Renderer(TargetWindow const &target_window);

    ~Renderer();

    void request_draw() { }

    /**
     * @brief Request the next image from the swapchain.
     */
    void acquire_next_image();

    /**
     * @brief Record received GPU commands to a command buffer.
     */
    void record_commands();

    /**
     * @brief Submit the recorded commands to the device queue.
     */
    void submit_commands();

    /**
     * @brief Request that the swapchain present the current image.
     */
    void present_image();

    Renderer() = delete;

    Renderer(Renderer &&) = delete;
    Renderer(const Renderer &) = delete;

    Renderer & operator=(Renderer &&) = delete;
    Renderer & operator=(const Renderer &) = delete;

private:
    vkInstance       *_instance;
    vkPhysicalDevice *_physical_device;
    vkDevice         *_device;
    vkSurface        *_surface;
    vkSwapchain      *_swapchain;
    vkRenderPass     *_render_pass;
    vkPipeline       *_pipeline;

    CubeMesh *_mesh;
    FPSCamera *_camera;

    vkDescriptorPool *_descriptor_pool;

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
     * @brief A collection of framebuffers associated with swapchain images
     */
    std::vector<vkFramebuffer *> _framebuffers;

    /**
     * @brief The index of the swapchain's next available image. This value is
     * also used to index Renderer's internal vkFrameSync objects.
     */
    uint32_t _next_image_index;

    /**
     * @brief Create the structures to synchronize commands with swapchain
     * images as well as the framebuffers
     */
    void _create_frame_data();

    /**
     * @brief Called from the destructor
     */
    void _destroy_frame_data();
};

} // namespace btx

#endif // BRASSTACKS_RENDERING_RENDERER_HPP