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
#endif // BTX platform

#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#define VMA_VULKLAN_VERSION BTX_VK_TARGET_VERSION
#include <vk_mem_alloc.h>

#endif // BRASSTACKS_PCH_HPP
