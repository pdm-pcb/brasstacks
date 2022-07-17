#ifndef BRASSTACKS_PLATFORM_GL_GLCONTEXTWGL_HPP
#define BRASSTACKS_PLATFORM_GL_GLCONTEXTWGL_HPP

#include "brasstacks/Engine/RenderContext.hpp"

#include <atomic>
#include <string>

namespace btx {

class TargetWindow;
class GLDebugger;

class GLContextWGL : public RenderContext {
public:
    void init()     override;
    void run()      override;
    void shutdown() override;

    void set_swap_interval(std::uint8_t interval) override;
    void set_clear_color(float r, float g, float b, float a) override;

    explicit GLContextWGL(const TargetWindow *window);
    ~GLContextWGL() = default;

    GLContextWGL() = delete;

    GLContextWGL(const GLContextWGL &&) = delete;
    GLContextWGL(GLContextWGL &)        = delete;

    GLContextWGL& operator=(const GLContextWGL &&) = delete;
    GLContextWGL& operator=(GLContextWGL &)        = delete;

private:
    ::HWND  _window;
    ::HDC   _device;
    ::HGLRC _context;

    std::atomic<bool> _running;

    GLDebugger *_debugger;

    std::string _window_title;

    void _driver_hooks();
    void _update_window_title();
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_GL_GLCONTEXTWGL_HPP