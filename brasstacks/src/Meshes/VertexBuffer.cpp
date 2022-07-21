#include "brasstacks/System/pch.hpp"
#include "brasstacks/Meshes/VertexBuffer.hpp"

#ifdef __linux__
    // ...?
#elif defined(_WIN32)
    #ifdef BTX_DIRECTX11
    // ...?
    #elif defined(BTX_OPENGL)
        #include "brasstacks/Platform/GL/GLVertexBuffer.hpp"
    #endif
#endif

namespace btx {

VertexBuffer * VertexBuffer::create(const VertexLayout::ElementList &elements) {
#ifdef __linux__
    // ...?
#elif defined(_WIN32)
    #ifdef BTX_DIRECTX11
    // ...?
    #elif defined(BTX_OPENGL)
        return new GLVertexBuffer(elements);
    #endif
#endif   
}

} //  namespace btx