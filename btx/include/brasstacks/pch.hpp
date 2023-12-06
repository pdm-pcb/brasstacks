#ifndef BRASSTACKS_PCH_HPP
#define BRASSTACKS_PCH_HPP

// Allow for the use of designated initializers
// https://github.com/KhronosGroup/Vulkan-Hpp#designated-initializers
#define VULKAN_HPP_NO_CONSTRUCTORS

// Prevent needing to load every function by hand
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#include <vulkan/vulkan.hpp>

#if defined(BTX_LINUX)
    #include <X11/Xlib.h>
    #include <vulkan/vulkan_xlib.h>
#elif defined(BTX_WINDOWS)
    #include <Windows.h>
    #undef min
    #undef max
    #undef near
    #undef far
    #undef ERROR

    #include <vulkan/vulkan_win32.h>
#endif

#include <cstdlib>
#include <format>
#include <memory>

#endif // BRASSTACKS_PCH_HPP