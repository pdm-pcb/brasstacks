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
class vkDescriptorPool;
class vkFrameSync;
class vkCmdBuffer;

class CubeMesh;

class FPSCamera;
class vkDevice;
class vkDescriptorPool;
class vkDescriptorSetLayout;
class vkDescriptorSet;
class vkBuffer;

class RenderPass;
class vkRenderPass;
class vkPipeline;
class vkFramebuffer;

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
    vkDescriptorPool *_descriptor_pool;

    vkRenderPass                *_render_pass;
    vkPipeline                  *_pipeline;
    std::vector<vkFramebuffer *> _framebuffers;

    struct PushConstant {
        vk::ShaderStageFlags const stage_flags = vk::ShaderStageFlagBits::eAll;
        size_t               const size_bytes  = 0;
        void                 const *data       = nullptr;
    };

    CubeMesh *_mesh;

    FPSCamera                     *_camera;
    vkDescriptorSetLayout         *_camera_ubo_layout;
    std::vector<vkDescriptorSet *> _camera_ubo_sets;
    std::vector<vkBuffer *>        _camera_ubos;

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
    uint32_t _next_image_index;

    /**
     * @brief Create the structures to synchronize commands with swapchain
     * images as well as the framebuffers
     */
    void _create_frame_sync();

    /**
     * @brief Called from the destructor
     */
    void _destroy_frame_sync();

    void _create_camera_resources();
    void _destroy_camera_resources();

    void _create_render_pass();
    void _destroy_render_pass();

    void _push_constants(vkCmdBuffer const &cmd_buffer,
                         std::vector<PushConstant> const &push_constants);
};

} // namespace btx

#endif // BRASSTACKS_RENDERING_RENDERER_HPP