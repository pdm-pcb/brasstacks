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

#include "brasstacks/Application/Application.hpp"
#include "brasstacks/Application/TargetWindow.hpp"
#include "brasstacks/Engine/Engine.hpp"
#include "brasstacks/Engine/RenderConfig.hpp"
#include "brasstacks/Utility/Logger.hpp"
#include "brasstacks/Utility/EnumRange.hpp"
#include "brasstacks/Utility/math.hpp"
#include "brasstacks/Events/Event.hpp"
#include "brasstacks/Events/EventPublisher.hpp"
#include "brasstacks/Events/EventListener.hpp"
#include "brasstacks/Events/WindowEvent.hpp"
#include "brasstacks/Events/MouseEvent.hpp"
#include "brasstacks/Events/KeyboardEvent.hpp"
#include "brasstacks/ECS/ECS.hpp"
#include "brasstacks/ECS/ECSView.hpp"
#include "brasstacks/System/Clock.hpp"
#include "brasstacks/System/Input/KeyCodes.hpp"
#include "brasstacks/System/glm.hpp"
#include "brasstacks/AssetLibraries/MeshLibrary.hpp"
#include "brasstacks/AssetLibraries/TextureLibrary.hpp"
#include "brasstacks/AssetLibraries/ShaderLibrary.hpp"
#include "brasstacks/Cameras/CameraBag.hpp"

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <vector>
#include <utility>
#include <algorithm>
#include <bitset>
#include <array>
#include <queue>
#include <unordered_map>

#endif // BRASSTACKS_UTILITY_PCH_HPP