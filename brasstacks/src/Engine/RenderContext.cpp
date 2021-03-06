#include "brasstacks/System/pch.hpp"
#include "brasstacks/Engine/RenderContext.hpp"

#include "brasstacks/Application/TargetWindow.hpp"

#ifdef __linux__
    // ...?
#elif defined(_WIN32)
    #ifdef BTX_OPENGL
        #include "brasstacks/Platform/GL/GLContextWGL.hpp"
    #elif defined(BTX_DIRECTX11)
        // ...?
    #endif
#endif

namespace btx {

RenderContext * RenderContext::_active = nullptr;

RenderContext * RenderContext::create() {
    if(TargetWindow::current() == nullptr) {
        BTX_ENGINE_ERROR("No target render window");
        return nullptr;
    }
#ifdef __linux__
    // ...?
#elif defined(_WIN32)
    #ifdef BTX_OPENGL
        return new GLContextWGL(TargetWindow::current());
    #elif defined(BTX_DIRECTX11)
        // ...?
    #endif
#endif
}

} // namespace btx