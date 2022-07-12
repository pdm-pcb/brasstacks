#ifndef BRASSTACKS_UTILITY_PCH_HPP
#define BRASSTACKS_UTILITY_PCH_HPP

// OS checks
#ifdef __linux__
    #include <X11/Xlib.h>
    #include <X11/Xutil.h>
    #include <X11/keysymdef.h>
    #include <X11/XKBlib.h>
    #include <X11/extensions/XInput2.h>
    #include <X11/extensions/Xfixes.h>

    #ifdef BTX_OPENGL
        #include <glad/glx.h>
        #define APIENTRY
    #endif
#elif defined(_WIN32)
    #include <winsdkver.h>
    #include <sdkddkver.h>
    #ifndef WINVER
        #define WINVER WINVER_MAXVER
    #endif
    #ifndef _WIN32_WINNT
        #define _WIN32_WINNT _WIN32_WINNT_MAXVER
    #endif

    #define VC_EXTRALEAN
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX

    #ifdef UNICODE
        #undef UNICODE
    #endif
    #ifdef _UNICODE
        #undef _UNICODE
    #endif

    #include <Windows.h>
    #include <Winuser.h>
    #include <CommCtrl.h>
    #include <hidusage.h>
    #include <shellscalingapi.h>

    #ifdef BTX_DIRECTX11
        #include <d3d11_4.h>
        #include <dxgi1_6.h>

        #ifdef DEBUG
            #include <dxgidebug.h>
        #endif
    #else
        #include <glad/wgl.h>
    #endif
#endif // OS Checks

#include "brasstacks/Utility/Logger.hpp"
#include "brasstacks/Utility/EnumRange.hpp"
#include "brasstacks/Events/Event.hpp"
#include "brasstacks/Events/EventPublisher.hpp"
#include "brasstacks/Events/EventListener.hpp"
#include "brasstacks/Events/WindowEvent.hpp"

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <system_error>

#endif // BRASSTACKS_UTILITY_PCH_HPP