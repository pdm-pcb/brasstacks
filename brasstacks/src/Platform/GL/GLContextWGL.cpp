#include "brasstacks/System/pch.hpp"
#include "brasstacks/Platform/GL/GLContextWGL.hpp"

#include "brasstacks/Application/TargetWindow.hpp"
#include "brasstacks/Platform/GL/GLVertexBuffer.hpp"
#include "brasstacks/Platform/GL/GLDebugger.hpp"

#include "brasstacks/ECS/ECS.hpp"
#include "brasstacks/ECS/ECSView.hpp"



#include "brasstacks/Meshes/MeshFlatColor.hpp"
#include "brasstacks/Shaders/ShaderFlatColor.hpp"
#include "brasstacks/Cameras/CameraBag.hpp"
#include "brasstacks/Engine/RenderConfig.hpp"
#include "brasstacks/Engine/RenderQueue.hpp"

namespace btx {

void GLContextWGL::run() {
    ECS *ecs = ECS::get_active();

    ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RenderQueue::begin_draw();
    Clock::frame_tick();
        
        for(auto &[shader, index] : RenderQueue::get_indices()) {
            auto camera = ecs->get<CameraComponent>(CameraBag::get_active());

            shader->bind();
            shader->update_camera(
                camera->view_matrix,
                camera->proj_matrix
            );

            for(auto id : RenderQueue::get_queue(index)) {
                auto render_c = ecs->get<RenderComponent>(id);

                shader->update_render_data(*render_c);
                render_c->mesh->bind_vertex_buffer();

                ::glDrawElements(
                    GL_TRIANGLES,
                    static_cast<GLsizei>(render_c->mesh->index_count()),
                    GL_UNSIGNED_INT,
                    0
                );
            }
        }

    RenderQueue::end_draw();
    Clock::frame_tock();

    ::SwapBuffers(_device);
    Clock::frame_delta_tock();

    _update_window_title();
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

    result = ::wglMakeCurrent(_device, _context);

	if(result == FALSE) {
		::MessageBox(nullptr, "wglMakeCurrent failed", "Error", MB_OK);
	}

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

void GLContextWGL::_update_window_title() {
    _window_title = fmt::format(
        "R:{:.3f}ms U:{:.3f}ms",
        Clock::frame_time(),
        Clock::update_time()
    );
    ::SetWindowText(_window, _window_title.c_str());
}

void GLContextWGL::shutdown() {
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
    _window   { static_cast<::HWND>(window->get_native()) },
    _device   { nullptr },
    _context  { nullptr },
    _debugger { nullptr },
    _window_title { "Brasstacks Engine " }
{ }

GLContextWGL::~GLContextWGL() {
    delete _debugger;
}

} // namespace btx