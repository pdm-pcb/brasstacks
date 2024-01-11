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
    #include <X11/Xlib.h>

    #define VK_USE_PLATFORM_XLIB_KHR
    #include <vulkan/vulkan.hpp>
    #include <vulkan/vulkan_xlib.h>
#elif defined(BTX_WINDOWS)
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
    #define _UNICODE

    #include <Windows.h>
    #include <hidusage.h>
    #include <winuser.h>

    #define VK_USE_PLATFORM_WIN32_KHR
    #include <vulkan/vulkan.hpp>
    #include <vulkan/vulkan_win32.h>
#endif // BTX platform

#include <cstdint>
#include <cstdlib>
#include <format>
#include <memory>
#include <string>
#include <string_view>
#include <cstring>
#include <list>
#include <functional>
#include <unordered_map>
#include <array>
#include <filesystem>
#include <fstream>
#include <queue>
#include <chrono>
#include <set>
#include <numbers>
#include <thread>
#include <mutex>

#if defined(BTX_DEBUG)
static std::filesystem::path const BTX_ASSET_PATH("../../demo/assets");
static std::string           const BTX_SHADER_EXT("-debug.spv");
#else
static std::filesystem::path const BTX_ASSET_PATH("assets");
static std::string           const BTX_SHADER_EXT("-release.spv");
#endif // BTX build config

#endif // BRASSTACKS_PCH_HPP