#include "brasstacks/System/pch.hpp"
#include "brasstacks/Engine/RenderContext.hpp"

#include "brasstacks/Application/TargetWindow.hpp"

#ifdef __linux__
    // ...?
#elif defined(_WIN32)
    #ifdef BTX_DIRECTX11
    // ...?
    #elif defined(BTX_OPENGL)
        #include "brasstacks/Platform/GL/GLContextWGL.hpp"
    #endif
#endif

namespace btx {

RenderContext * RenderContext::create() {
    if(TargetWindow::current() == nullptr) {
        BTX_ENGINE_ERROR("No target render window");
        return nullptr;
    }
#ifdef __linux__
    // ...?
#elif defined(_WIN32)
    #ifdef BTX_DIRECTX11
    // ...?
    #elif defined(BTX_OPENGL)
        return new GLContextWGL(TargetWindow::current());
        // ::glEnable(GL_DEPTH_TEST);
        // ::glEnable(GL_CULL_FACE);
    #endif
#endif
}

} // namespace btx