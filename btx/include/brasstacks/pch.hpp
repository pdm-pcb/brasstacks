#ifndef BRASSTACKS_PCH_HPP
#define BRASSTACKS_PCH_HPP

// Allow for the use of designated initializers
// https://github.com/KhronosGroup/Vulkan-Hpp#designated-initializers
#define VULKAN_HPP_NO_CONSTRUCTORS

// Prevent needing to load every function by hand
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

// I'll check the return values
#define VULKAN_HPP_NO_EXCEPTIONS

#if defined(BTX_LINUX)
    #define VK_USE_PLATFORM_XLIB_KHR
    #include <vulkan/vulkan.hpp>
    #include <vulkan/vulkan_xlib.h>

    #define GLFW_EXPOSE_NATIVE_X11
#elif defined(BTX_WINDOWS)
    #define NOMINMAX

    #include <dwmapi.h>

    #define VK_USE_PLATFORM_WIN32_KHR
    #include <vulkan/vulkan.hpp>
    #include <vulkan/vulkan_win32.h>

    #define GLFW_EXPOSE_NATIVE_WIN32
#endif // BTX platform

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <cstdint>
#include <cstdlib>
#include <format>
#include <memory>
#include <string>
#include <string_view>
#include <numbers>
#include <limits>
#include <set>
#include <filesystem>
#include <fstream>
#include <queue>

#if defined(BTX_DEBUG)
    static std::filesystem::path const BTX_ASSET_PATH("../../demo/assets");
    static std::string           const BTX_SHADER_EXT("-debug.spv");
#else
    static std::filesystem::path const BTX_ASSET_PATH("assets");
    static std::string           const BTX_SHADER_EXT("-release.spv");
#endif // BTX build config

#endif // BRASSTACKS_PCH_HPP