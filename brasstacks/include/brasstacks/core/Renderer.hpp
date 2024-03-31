#ifndef BRASSTACKS_CORE_RENDERER_HPP
#define BRASSTACKS_CORE_RENDERER_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/platform/vulkan/swapchain/vkSurface.hpp"
#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/vmaAllocator.hpp"
#include "brasstacks/platform/vulkan/swapchain/vkFrameSync.hpp"
#include "brasstacks/platform/vulkan/swapchain/vkSwapchain.hpp"

namespace btx {

class Application;
class vkCmdBuffer;
class vkDescriptorPool;

class Renderer final {
public:
    static void init(Application *application);
    static void shutdown();
    static void run();
    static void recreate_swapchain();

    static inline void wait_device_idle() { _device->wait_idle(); }

    static inline auto const & device()    { return *_device; }
    static inline auto const & swapchain() { return *_swapchain; }

    static inline auto render_surface_aspect_ratio() {
        return _swapchain->aspect_ratio();
    }

    static inline auto image_index() { return _image_index; }
    static inline auto const & cmd_buffer() {
        return _frame_sync[_image_index]->cmd_buffer();
    }

    static inline auto & descriptor_pool() { return *_descriptor_pool; }

    Renderer() = delete;
    ~Renderer() = delete;

    Renderer(Renderer &&) = delete;
    Renderer(Renderer const &) = delete;

    Renderer & operator=(Renderer &&) = delete;
    Renderer & operator=(Renderer const &) = delete;

private:
    static Application *_application;

    static std::unique_ptr<vkSurface>   _surface;
    static std::unique_ptr<vkDevice>    _device;
    static std::unique_ptr<vkSwapchain> _swapchain;

    static std::vector<std::unique_ptr<vkFrameSync>> _frame_sync;

    static uint32_t _image_index;

    static std::unique_ptr<vkDescriptorPool> _descriptor_pool;

    [[nodiscard]] static uint32_t _acquire_next_image();

    static void _begin_recording();
    static void _end_recording();
    static void _submit_commands();

    [[nodiscard]] static bool _present_image();

    static void _create_surface();
    static void _select_physical_device();
    static void _create_device();
    static void _create_allocator(uint32_t const api_version);
    static void _create_swapchain();
    static void _create_frame_sync();

    static void _destroy_swapchain();
    static void _destroy_frame_sync();
};

} // namespace btx

#endif // BRASSTACKS_CORE_RENDERER_HPP