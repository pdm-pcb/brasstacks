#ifndef BRASSTACKS_PLATFORM_GL_GLCONTEXTWGL_HPP
#define BRASSTACKS_PLATFORM_GL_GLCONTEXTWGL_HPP

#include "brasstacks/Engine/RenderContext.hpp"

#include <string>
#include <mutex>

namespace btx {

class TargetWindow;
class GLDebugger;

class Profiler;

class GLContextWGL : public RenderContext {
public:
    void init()     override;
    void run()      override;
    void shutdown() override;

    void set_swap_interval(std::uint8_t interval) override;
    void set_clear_color(float r, float g, float b, float a) override;

    void make_current();
    void release_context();

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

    GLDebugger *_debugger;

    std::string _window_title;

    std::mutex _context_mutex;


    Profiler *_draw_perf;
    Profiler *_bufswap_perf;

    void _driver_hooks();
    void _update_window_title();
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_GL_GLCONTEXTWGL_HPP