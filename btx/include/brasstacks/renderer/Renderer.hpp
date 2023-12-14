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
class vkPipeline;

class vkCmdPool;
class vkCmdBuffer;

class vkFrame;

class Renderer {
public:
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
    vkPipeline       *_pipeline;

    vkCmdPool        *_cmd_pool;
    vkCmdBuffer      *_cmd_buffer;

    std::vector<vkFrame *> _frames;
};

} // namespace btx

#endif // BRASSTACKS_RENDERER_RENDERER_HPP