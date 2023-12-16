#ifndef BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKDEVICE_HPP
#define BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKDEVICE_HPP

#include "brasstacks/pch.hpp"

#include "brasstacks/platform/vulkan/devices/vkQueue.hpp"

namespace btx {

class vkPhysicalDevice;

class vkDevice final {
public:
    void wait_idle() const;

    inline auto const& native()    const { return _handle; }
    inline auto const& queue() const { return *_queue; }

    using Layers = std::vector<char const *>;
    explicit vkDevice(vkPhysicalDevice const &adapter,
                      Layers const &layers = { });
    ~vkDevice();

    vkDevice() = delete;

    vkDevice(vkDevice &&) = delete;
    vkDevice(const vkDevice &) = delete;

    vkDevice & operator=(vkDevice &&) = delete;
    vkDevice & operator=(const vkDevice &) = delete;

private:
    vkQueue *_queue;
    vk::Device _handle;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKDEVICE_HPP