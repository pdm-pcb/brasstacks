#ifndef BRASSTACKS_PCH_HPP
#define BRASSTACKS_PCH_HPP

#include <cstdint>
#include <cstdlib>
#include <memory>
#include <string>
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
    // Start with the old standbys
    #define WIN32_LEAN_AND_MEAN
    #define VC_EXTRALEAN

    // And now disable a bunch of features one by one
    #define NOGDICAPMASKS    // Color and other GDI stuff
    #define NOMENUS
    #define NOICONS
    #define NORASTEROPS      // Rasterization functions
    #define NOCLIPBOARD
    #define NOCOLOR          // UI color
    #define NODRAWTEXT       // Drawing formatted text
    #define NOMB             // Message box
    #define NOMEMMGR         // Global/local memory managers
    #define NOMETAFILE       // GDI/clipboard file format
    #define NOMINMAX         // Bothersome min and max macros
    #define NOOPENFILE       // File manipulation
    #define NOSCROLL
    #define NOSERVICE        // Windows services
    #define NOSOUND
    #define NOTEXTMETRIC     // Font info
    #define NOWH             // Windows hooks
    #define NOCOMM
    #define NOHELP
    #define NODEFERWINDOWPOS // More multi-window functionality
    #define NOMCX            // Modem configuration extensions

    #define UNICODE

    #define VK_USE_PLATFORM_WIN32_KHR
    #include <vulkan/vulkan.hpp>
    #include <vulkan/vulkan_win32.h>

    #include <dwmapi.h>
#endif // BTX platform

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

auto constexpr VK_TARGET_VERSION = VK_API_VERSION_1_2;

#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#define VMA_VULKLAN_VERSION VK_TARGET_VERSION
#include <vk_mem_alloc.h>

#ifdef BTX_LINUX
    #define GLFW_EXPOSE_NATIVE_X11
#elif BTX_WINDOWS
    #define GLFW_EXPOSE_NATIVE_WIN32
#endif // BTX platform

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#ifdef BTX_DEBUG
    static std::filesystem::path const BTX_ASSET_PATH("demo/assets");
    static std::string           const BTX_SHADER_EXT("-debug.spv");
#else
    static std::filesystem::path const BTX_ASSET_PATH("demo/assets");
    static std::string           const BTX_SHADER_EXT("-release.spv");
#endif // BTX build config

#endif // BRASSTACKS_PCH_HPP