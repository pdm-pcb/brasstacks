/**
 * @file vkQueue.hpp
 * @brief A wrapper for the Vulkan logical device queue
 */

#ifndef BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKQUEUE_HPP
#define BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKQUEUE_HPP

#include "brasstacks/pch.hpp"

namespace btx {

/**
 * @brief A wrapper for the Vulkan logical device queue
 *
 * This class holds on to a device queue handle as well as that queue's family
 * index, which is referred to in various places.
 */
class vkQueue final {
public:
    vkQueue();
    ~vkQueue() = default;

    void set_family_index(uint32_t const index);

    /**
     * @brief Return the queue family index
     * @return uint32_t
     */
    inline auto family_index() const { return _family_index; }

    /**
     * @brief Return the native Vulkan handle
     * @return vk::Queue const&
     */
    inline auto const& native() const { return _handle;}

    vkQueue(vkQueue &&) = delete;
    vkQueue(const vkQueue &) = delete;

    vkQueue& operator=(vkQueue &&) = delete;
    vkQueue& operator=(const vkQueue &) = delete;

private:
    /**
     * @brief The native Vulkan handle
     */
    vk::Queue _handle;

    /**
     * @brief This queue's family index
     */
    uint32_t _family_index;

};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKQUEUE_HPP