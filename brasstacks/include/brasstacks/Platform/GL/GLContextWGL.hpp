#ifndef BRASSTACKS_PLATFORM_GL_GLCONTEXTWGL_HPP
#define BRASSTACKS_PLATFORM_GL_GLCONTEXTWGL_HPP

#include "brasstacks/Engine/RenderContext.hpp"

namespace btx {

class TargetWindow;

class GLContextWGL : public RenderContext {
public:
    void init() override;
    void shutdown() override;
    void swap_buffers() override;
    void set_swap_interval(uint8_t interval) override;

    explicit GLContextWGL(const TargetWindow *window);
    ~GLContextWGL();

    GLContextWGL() = delete;

    GLContextWGL(const GLContextWGL &&) = delete;
    GLContextWGL(GLContextWGL &)        = delete;

    GLContextWGL& operator=(const GLContextWGL &&) = delete;
    GLContextWGL& operator=(GLContextWGL &)        = delete;

private:
    ::HWND  _window;
    ::HDC   _device;
    ::HGLRC _context;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_GL_GLCONTEXTWGL_HPP