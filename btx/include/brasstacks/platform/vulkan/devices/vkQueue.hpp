/**
 * @file vkQueue.hpp
 * @brief A wrapper for the Vulkan logical device queue
 */

#ifndef BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKQUEUE_HPP
#define BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKQUEUE_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkDevice;

/**
 * @brief A wrapper for the Vulkan logical device queue
 *
 * This class holds on to a device queue handle as well as that queue's family
 * index, which is referred to in various places.
 */
class vkQueue final {
public:

    /**
     * @brief Return the queue family index
     * @return uint32_t
     */
    inline auto index() const { return _index; }

    /**
     * @brief Return the native Vulkan handle
     * @return vk::Queue const&
     */
    inline auto const& native() const { return _handle;}

    /**
     * @brief Construct the vk Queue object.
     * @param device An established Vulkan logical device
     * @param queue_family_index A queue family index from a vkPhysicalDevice
     */
    vkQueue(vkDevice const &device, uint32_t const queue_family_index);

    ~vkQueue() = default;

    vkQueue() = delete;

    vkQueue(vkQueue &&) = delete;
    vkQueue(const vkQueue &) = delete;

    vkQueue& operator=(vkQueue &&) = delete;
    vkQueue& operator=(const vkQueue &) = delete;

private:
    /**
     * @brief This queue's family index
     */
    uint32_t const _index;

    /**
     * @brief The native Vulkan handle
     */
    vk::Queue _handle;

    /**
     * @brief The vkDevice from which this queue was grabbed
     */
    vkDevice const &_device;

};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKQUEUE_HPP