#ifndef BRASSTACKS_RENDERER_RENDERER_HPP
#define BRASSTACKS_RENDERER_RENDERER_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class TargetWindow;

class vkInstance;
class vkPhysicalDevice;
class vkDevice;
class vkSurface;
class vkSwapchain;
class vkRenderPass;
class vkPipeline;

class vkFrame;

class Renderer {
public:
    void request_draw() { }

    void acquire_next_frame();
    void record_commands();
    void submit_commands();
    void present_image();

    explicit Renderer(TargetWindow const &target_window);
    ~Renderer();

    Renderer() = delete;

    Renderer(Renderer &&) = delete;
    Renderer(const Renderer &) = delete;

    Renderer & operator=(Renderer &&) = delete;
    Renderer & operator=(const Renderer &) = delete;

private:
    vkInstance       *_instance;
    vkPhysicalDevice *_adapter;
    vkDevice         *_device;
    vkSurface        *_surface;
    vkSwapchain      *_swapchain;
    vkRenderPass     *_render_pass;
    vkPipeline       *_pipeline;

    std::queue<vk::Semaphore> _image_acquire_sems;
    std::vector<vkFrame *> _frames;
    uint32_t _next_image_index;

    void _create_frame_data();
};

} // namespace btx

#endif // BRASSTACKS_RENDERER_RENDERER_HPP