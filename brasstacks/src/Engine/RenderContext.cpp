#include "brasstacks/System/pch.hpp"
#include "brasstacks/Engine/RenderContext.hpp"

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

RenderContext * RenderContext::create(const TargetWindow *window) {
#ifdef __linux__
    // ...?
#elif defined(_WIN32)
    #ifdef BTX_DIRECTX11
    // ...?
    #elif defined(BTX_OPENGL)
        RenderContext *context = new GLContextWGL(window);
        context->init();
        ::glEnable(GL_DEPTH_TEST);
        ::glEnable(GL_CULL_FACE);
        return context;
    #endif
#endif
}

} // namespace btx