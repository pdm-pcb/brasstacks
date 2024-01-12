#ifndef BRASSTACKS_CORE_RENDERER_HPP
#define BRASSTACKS_CORE_RENDERER_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/config/RenderConfig.hpp"
#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"

namespace btx {

class TargetWindow;
class vkSurface;
class vkSwapchain;
class vkFrameSync;
class vkCmdBuffer;

class Renderer final {
public:
    explicit Renderer(TargetWindow const &target_window);
    ~Renderer();

    uint32_t acquire_next_image();
    vkCmdBuffer const & begin_recording();
    void end_recording();
    void submit_commands();
    [[nodiscard]] bool present_image();

    inline void wait_device_idle() const { _device->wait_idle(); }

    void recreate_swapchain();

    inline auto const & device()    const { return *_device; }
    inline auto const & swapchain() const { return *_swapchain; }

    Renderer() = delete;

    Renderer(Renderer &&) = delete;
    Renderer(Renderer const &) = delete;

    Renderer & operator=(Renderer &&) = delete;
    Renderer & operator=(Renderer const &) = delete;

private:
    TargetWindow const &_target_window;

    vkSurface   *_surface;
    vkDevice    *_device;
    vkSwapchain *_swapchain;

    std::queue<vk::Semaphore>  _image_acquire_sems;
    std::vector<vkFrameSync *> _frame_sync;

    uint32_t _image_index;

    void _create_surface();
    void _select_physical_device();
    void _create_device();
    void _create_swapchain();
    void _create_frame_sync();

    void _destroy_swapchain();
    void _destroy_frame_sync();
};

} // namespace btx

#endif // BRASSTACKS_CORE_RENDERER_HPP