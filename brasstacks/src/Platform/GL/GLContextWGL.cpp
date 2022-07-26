#include "brasstacks/System/pch.hpp"
#include "brasstacks/Platform/GL/GLContextWGL.hpp"

#include "brasstacks/Application/TargetWindow.hpp"
#include "brasstacks/Platform/GL/GLVertexBuffer.hpp"
#include "brasstacks/Platform/GL/GLDebugger.hpp"

#include "brasstacks/ECS/ECS.hpp"
#include "brasstacks/ECS/ECSView.hpp"
#include "brasstacks/Cameras/CameraBag.hpp"
#include "brasstacks/Engine/RenderQueue.hpp"

namespace btx {

void GLContextWGL::run() {
    std::uint32_t entity_count = 0;

    ECS *ecs = ECS::get_active();

    ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RenderQueue::begin_draw();
    Clock::frame_tick();

_draw_perf->start();        
        for(auto [shader, index] : RenderQueue::get_indices()) {
            auto camera = ecs->get<cCamera>(CameraBag::get_active());

            shader->bind();
            shader->update_camera(
                camera->view_matrix,
                camera->persp_proj
            );
            shader->update_per_frame();

            for(auto id : RenderQueue::get_queue(index)) {
                shader->update_per_object(id);
                
                auto render_c = ecs->get<cRender>(id);
                render_c->mesh->bind_vertex_buffer();

                ::glDrawElements(
                    GL_TRIANGLES,
                    static_cast<GLsizei>(render_c->mesh->index_count()),
                    GL_UNSIGNED_INT, 0
                );
                ++entity_count;
            }
        }

        ScreenLog::write_line(
            "Hello, World!\0",
            5.0f, RenderConfig::window_y_res - 27.0f, 1.0f
        );

    RenderQueue::end_draw();
    Clock::frame_tock();

_draw_perf->stop(0.0f);
_bufswap_perf->start();
    ::SwapBuffers(_device);
_bufswap_perf->stop(0.0f);

    Clock::frame_delta_tock();

    // _update_window_title();
}

void GLContextWGL::init() {
    // first, get a context so we can get a context
    _driver_hooks();

    //
    // next, let's ask for a modern pixel format
    //
    int pixel_attributes[] = {
        WGL_SUPPORT_OPENGL_ARB,     GL_TRUE,
        WGL_DRAW_TO_WINDOW_ARB,     GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB,      GL_TRUE,
        WGL_COLOR_BITS_ARB,         32,
        WGL_DEPTH_BITS_ARB,         24,
        WGL_STENCIL_BITS_ARB,        8,
        WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
        WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
        0
    };

    // retrieve the win32 device context
    _device = ::GetDC(_window);

    int pixel_format;
    ::UINT formats_found;     // should I be enumerating these?
    ::BOOL result = ::wglChoosePixelFormatARB(
        _device,
        pixel_attributes,
        nullptr,
        1,                // max formats
        &pixel_format,
        &formats_found
    );

	if(result == FALSE) {
		::MessageBox(nullptr, "wglChoosePixelFormatARB failed", "Error", MB_OK);
	}

    // set the format we requested
    result = ::SetPixelFormat(_device, pixel_format, nullptr);

	if(result == FALSE) {
		::MessageBox(nullptr, "SetPixelFormat failed", "Error", MB_OK);
	}

    //
    // the pixel format is sorted, so now on to the rendering context
    //

    int context_attributes[] {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 6,
#ifdef DEBUG
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
#endif
        WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };

    _context = ::wglCreateContextAttribsARB(_device, 0, context_attributes);

    if(_context == nullptr) {
        ::MessageBox(
            nullptr, "wglCreateContextAttribsARB failed",
            "Error", MB_OK
        );
    }

    make_current();

    ::glEnable(GL_DEPTH_TEST);
    ::glEnable(GL_CULL_FACE);


#ifdef DEBUG
    // since the context is good, create the debugging helper
    _debugger = new GLDebugger;
#endif

    BTX_ENGINE_INFO("Initialized WGL context:\n    {}\n    {}\n    {}",
                    ::glGetString(GL_RENDERER), ::glGetString(GL_VENDOR),
                    ::glGetString(GL_VERSION));
}

void GLContextWGL::_driver_hooks() {
    //
    // First things first: let's get a window up that we can attach an OpenGL
    // context to
    //
    auto hinstance = static_cast<::HINSTANCE>(GetModuleHandle(nullptr));
    ::HWND window = CreateWindow(
        "static",   // window class
        "",         // window name
        WS_POPUP,   // window style
        0, 0,       // x,y coordinates
        1, 1,       // width/height
        nullptr,    // parent
        nullptr,    // menu
        hinstance,  // "instance"
        nullptr     // lpParam
    );

    // grab the device from the new window
    ::HDC device = ::GetDC(window);

    //
    // Next up: get the basic pixel format
    //
    ::PIXELFORMATDESCRIPTOR pfd { };
    pfd.nSize      = sizeof(::PIXELFORMATDESCRIPTOR);
    pfd.nVersion   = 1u;
    pfd.dwFlags    = PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32u;
    pfd.cDepthBits = 24u;
    pfd.iLayerType = PFD_MAIN_PLANE;

    // enumerate and select the format we specified
    int pixel_format = ::ChoosePixelFormat(device, &pfd);
    if(pixel_format == 0) {
        ::MessageBox(nullptr, "ChoosePixelFormat failed", "Error", MB_OK);
    }

    // set the format
    BOOL result = ::SetPixelFormat(device, pixel_format, nullptr);
    if(result == FALSE) {
        ::MessageBox(nullptr, "SetPixelFormat failed", "Error", MB_OK);
    }

    //
    // Time to create a legacy context
    //
    ::HGLRC context = ::wglCreateContext(device);
    if(context == nullptr) {
        ::MessageBox(nullptr, "wglCreateContext failed", "Error", MB_OK);
    }

    result = ::wglMakeCurrent(device, context);
    if(result == FALSE) {
        ::MessageBox(nullptr, "wglMakeCurrent failed", "Error", MB_OK);
    }

    //
    // Now leverage glad2 so I don't have to type all of that out
    //
    if(!gladLoaderLoadGL()) {
        ::MessageBox(nullptr, "Unable to load GL via GLAD", "Error", MB_OK);
    }

    if(!gladLoaderLoadWGL(device)) {
        ::MessageBox(nullptr, "Unable to load WGL via GLAD", "Error", MB_OK);
    }

    // Clean up after ourselves
    ::wglMakeCurrent(device, nullptr);
    ::wglDeleteContext(context);
    ::ReleaseDC(window, device);
    ::DestroyWindow(window);
}

void GLContextWGL::make_current() {
    _context_mutex.lock();
    ::BOOL result = ::wglMakeCurrent(_device, _context);

	if(result == FALSE) {
		::MessageBox(nullptr, "wglMakeCurrent acquire failed", "Error", MB_OK);
	}
}

void GLContextWGL::release_context() {
    ::BOOL result = ::wglMakeCurrent(nullptr, nullptr);
    _context_mutex.unlock();

	if(result == FALSE) {
		::MessageBox(nullptr, "wglMakeCurrent release failed", "Error", MB_OK);
	}
}

void GLContextWGL::_update_window_title() {
    _window_title = fmt::format(
        "R:{:.3f} U:{:.3f}",
        Clock::frame_time(),
        Clock::update_time()
    );
    ::SetWindowText(_window, _window_title.c_str());
}

void GLContextWGL::shutdown() {
    release_context();
    BTX_ENGINE_TRACE("Shutting down WGL");
	::wglMakeCurrent(_device, nullptr);
	::wglDeleteContext(_context);
    ::ReleaseDC(_window, _device);
}

void GLContextWGL::set_swap_interval(std::uint8_t interval) {
    BTX_ENGINE_TRACE("Swap interval set to {}", interval);
    ::wglSwapIntervalEXT(interval);
}

void GLContextWGL::set_clear_color(float r, float g, float b, float a) {
    BTX_ENGINE_TRACE(
        "Clear color set to ({:0.2f}, {:0.2f}, {:0.2f}, {:0.2f})",
        r, g, b, a
    );
    ::glClearColor(r, g, b, a);
}

GLContextWGL::GLContextWGL(const TargetWindow *window) :
    _window       { static_cast<::HWND>(window->get_native()) },
    _device       { nullptr },
    _context      { nullptr },
    _debugger     { nullptr },
    _draw_perf  { Profiler::spawn("gpu commands") },
    _bufswap_perf  { Profiler::spawn("buffer swap") },
    _window_title { "Brasstacks Engine " }
{ }

GLContextWGL::~GLContextWGL() {
    delete _debugger;
    delete _draw_perf;
    delete _bufswap_perf;
}

} // namespace btx