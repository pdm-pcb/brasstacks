#include "brasstacks/brasstacks.hpp"
#include "brasstacks/core/TargetWindow.hpp"

#include "brasstacks/events/EventBus.hpp"
#include "brasstacks/events/window_events.hpp"
#include "brasstacks/events/keyboard_events.hpp"

namespace btx {

GLFWToBTXKeys TargetWindow::_keymap { };

// =============================================================================
TargetWindow::TargetWindow(std::string_view const app_name) :
    _window  { nullptr },
    _screen_size { },
    _screen_center { },
    _window_size { },
    _window_position { }
{
    if(::glfwInit() == 0) {
        BTX_CRITICAL("Failed to initialize GLFW");
        return;
    }

    BTX_INFO("Initialized GLFW {:s}", ::glfwGetVersionString());

    ::glfwSetErrorCallback(TargetWindow::_error_callback);

    _calc_window_dimensions();

    ::glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    ::glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    _window = ::glfwCreateWindow(
        static_cast<int>(_window_size.width),
        static_cast<int>(_window_size.height),
        app_name.data(),
        nullptr,
        nullptr
    );

    if(_window == nullptr) {
        BTX_CRITICAL("GLFW window creation failed");
        return;
    }

    ::glfwSetWindowPos(_window, _window_position.x, _window_position.y);

    ::glfwSetKeyCallback(_window, TargetWindow::_key_callback);
    ::glfwSetWindowSizeCallback(_window, TargetWindow::_window_size_callback);
    ::glfwSetWindowIconifyCallback(_window,
                                   TargetWindow::_window_iconify_callback);

#ifdef BTX_WINDOWS
    ::BOOL value = TRUE;
    ::DwmSetWindowAttribute(
        this->native(),
        DWMWA_USE_IMMERSIVE_DARK_MODE,
        &value,
        sizeof(value)
    );
#endif

}

// =============================================================================
TargetWindow::~TargetWindow() {
    ::glfwDestroyWindow(_window);
    ::glfwTerminate();
}

// =============================================================================
void TargetWindow::poll_events() {
    ::glfwPollEvents();

    if(::glfwWindowShouldClose(_window)) {
        EventBus::publish(WindowCloseEvent { });
    }
}

// =============================================================================
void TargetWindow::_calc_window_dimensions() {
    auto const *video_mode = ::glfwGetVideoMode(::glfwGetPrimaryMonitor());

    auto const width  = static_cast<float>(video_mode->width)  * 0.75f;
    auto const height = static_cast<float>(video_mode->height) * 0.75f;

    _window_size = {
        .width  = static_cast<uint32_t>(width),
        .height = static_cast<uint32_t>(height),
    };

    auto const center_x = static_cast<float>(video_mode->width) * 0.5f;
    auto const center_y = static_cast<float>(video_mode->height) * 0.5f;

    _window_position = {
        .x = static_cast<int32_t>(center_x - (width * 0.5f)),
        .y = static_cast<int32_t>(center_y - (height * 0.5f)),
    };
}

// =============================================================================
void TargetWindow::_error_callback(int code, char const *message) {
    BTX_ERROR("GLFW Error {}: '{:s}'", code, message);
}

// =============================================================================
void TargetWindow::_key_callback([[maybe_unused]] GLFWwindow *window,
                                 [[maybe_unused]] int key,
                                 [[maybe_unused]] int scancode,
                                 [[maybe_unused]] int action,
                                 [[maybe_unused]] int mods)
{
    if(action == GLFW_PRESS) {
        EventBus::publish(KeyPressEvent { .code = _keymap.translate(key) });
    }
    else if(action == GLFW_RELEASE) {
        EventBus::publish(KeyReleaseEvent { .code = _keymap.translate(key) });
    }
}

// =============================================================================
void TargetWindow::_window_size_callback([[maybe_unused]] GLFWwindow* window,
                                         [[maybe_unused]] int width,
                                         [[maybe_unused]] int height)
{
    EventBus::publish(WindowSizeEvent {
        .size = {
            .width  = static_cast<uint32_t>(width),
            .height = static_cast<uint32_t>(height)
        }
    });
}

// =============================================================================
void TargetWindow::_window_iconify_callback([[maybe_unused]] GLFWwindow* window,
                                            [[maybe_unused]] int iconified)
{
    if(iconified == GLFW_TRUE) {
        EventBus::publish(WindowMinimizeEvent { });
    }
    else {
        EventBus::publish(WindowRestoreEvent { });
    }
}

} // namespace btx