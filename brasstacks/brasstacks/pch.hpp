#ifndef BRASSTACKS_PCH_HPP
#define BRASSTACKS_PCH_HPP

#include <cstdint>
#include <cstdlib>
#include <string>
#include <span>
#include <optional>
#include <cmath>
#include <numbers>
#include <limits>
#include <set>
#include <list>
#include <filesystem>
#include <fstream>
#include <queue>
#include <stack>

// Allow for the use of designated initializers
// https://github.com/KhronosGroup/Vulkan-Hpp#designated-initializers
#define VULKAN_HPP_NO_CONSTRUCTORS

// No need to load every function by hand
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#ifdef BTX_LINUX
    #define VK_USE_PLATFORM_XLIB_KHR
    #include <vulkan/vulkan.hpp>
    #include <vulkan/vulkan_xlib.h>
#elif BTX_WINDOWS
    #define NOMINMAX

    #define VK_USE_PLATFORM_WIN32_KHR
    #include <vulkan/vulkan.hpp>
    #include <vulkan/vulkan_win32.h>
#endif // BTX platform

// #include <spirv_reflect.h>

// #define VMA_STATIC_VULKAN_FUNCTIONS 0
// #define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
// #define VMA_VULKLAN_VERSION BTX_VK_TARGET_VERSION
// #include <vk_mem_alloc.h>

namespace btx {

struct Size {
    int32_t width = 0;
    int32_t height = 0;
};

struct Offset {
    uint32_t x = 0u;
    uint32_t y = 0u;
};

} // namespace btx

#endif // BRASSTACKS_PCH_HPP
