#ifndef BRASSTACKS_CORE_RENDERER_HPP
#define BRASSTACKS_CORE_RENDERER_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/swapchain/vkFrameSync.hpp"
#include "brasstacks/platform/vulkan/swapchain/vkSwapchain.hpp"

namespace btx {

class Application;
class vkSurface;
class vkCmdBuffer;

class Renderer final {
public:
    explicit Renderer(Application &application);
    ~Renderer() = default;

    void run();
    void recreate_swapchain();
    void shutdown();

    inline void wait_device_idle() const { _device->wait_idle(); }

    inline auto const & device()    const { return *_device; }
    inline auto const & swapchain() const { return *_swapchain; }

    inline auto render_surface_aspect_ratio() const {
        return _swapchain->aspect_ratio();
    }

    inline auto image_index() const { return _image_index; }
    inline auto const & cmd_buffer() const {
        return _frame_sync[_image_index]->cmd_buffer();
    }

    Renderer() = delete;

    Renderer(Renderer &&) = delete;
    Renderer(Renderer const &) = delete;

    Renderer & operator=(Renderer &&) = delete;
    Renderer & operator=(Renderer const &) = delete;

private:
    Application &_application;

    vkSurface   *_surface;
    vkDevice    *_device;
    vkSwapchain *_swapchain;

    std::vector<vkFrameSync *> _frame_sync;

    uint32_t _image_index;

    void _acquire_next_image();
    void _begin_recording();
    void _end_recording();
    void _submit_commands();
    [[nodiscard]] bool _present_image();

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