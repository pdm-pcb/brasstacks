#include "brasstacks/brasstacks.hpp"
#include "brasstacks/core/TargetWindow.hpp"

#include "brasstacks/events/EventBus.hpp"
#include "brasstacks/events/window_events.hpp"
#include "brasstacks/events/keyboard_events.hpp"
#include "brasstacks/events/mouse_events.hpp"
#include "brasstacks/platform/input/GLFWToBTXKeys.hpp"

namespace btx {

double TargetWindow::_last_cursor_x { 0.0 };
double TargetWindow::_last_cursor_y { 0.0 };

// =============================================================================
TargetWindow::TargetWindow(std::string_view const app_name) :
    _window          { nullptr },
    _screen_size     { },
    _screen_center   { },
    _window_size     { },
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
    ::glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

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

    if(::glfwRawMouseMotionSupported() != GLFW_TRUE) {
        BTX_CRITICAL("Raw mouse input not supported by this platform.");
        return;
    }

    ::glfwSetWindowPos(_window, _window_position.x, _window_position.y);

    ::glfwSetWindowSizeLimits(
        _window,
        320, 240, // Minimum dimensions
        GLFW_DONT_CARE, GLFW_DONT_CARE // Maximum dimensions
    );

    ::glfwSetKeyCallback(_window, TargetWindow::_key_callback);
    ::glfwSetMouseButtonCallback(_window, TargetWindow::_mouse_button_callback);
    ::glfwSetCursorPosCallback(_window, _mouse_move_callback);
    ::glfwSetWindowSizeCallback(_window, TargetWindow::_window_size_callback);
    ::glfwSetWindowIconifyCallback(_window,
                                   TargetWindow::_window_iconify_callback);
}

// =============================================================================
TargetWindow::~TargetWindow() {
    ::glfwDestroyWindow(_window);
    ::glfwTerminate();
}

// =============================================================================
void TargetWindow::capture_mouse() {
    ::glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    ::glfwSetInputMode(_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    ::glfwGetCursorPos(_window, &_last_cursor_x, &_last_cursor_y);
}

// =============================================================================
void TargetWindow::release_mouse() {
    ::glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    ::glfwSetInputMode(_window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
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
    auto const code = GLFWToBTXKeys::translate(key);
    if(action == GLFW_PRESS) {
        EventBus::publish(KeyPressEvent(code));
    }
    else if(action == GLFW_RELEASE) {
        EventBus::publish(KeyReleaseEvent(code));
    }
}

// =============================================================================
void TargetWindow::_mouse_move_callback([[maybe_unused]] GLFWwindow* window,
                                        [[maybe_unused]] double x,
                                        [[maybe_unused]] double y)
{
    // static double x_offset = 0.0;
    // static double y_offset = 0.0;

    // x_offset = x - _last_cursor_x;
    // y_offset = y - _last_cursor_y;

    // _last_cursor_x = x;
    // _last_cursor_y = y;

    // EventBus::publish(
    //     MouseMoveEvent(static_cast<int32_t>(x_offset),
    //                    static_cast<int32_t>(y_offset))
    // );
}

// =============================================================================
void TargetWindow::_mouse_button_callback([[maybe_unused]] GLFWwindow* window,
                                          [[maybe_unused]] int button,
                                          [[maybe_unused]] int action,
                                          [[maybe_unused]] int mods)
{
    auto const code = GLFWToBTXKeys::translate(button);

    if(action == GLFW_PRESS) {
        EventBus::publish(MouseButtonPressEvent(code));
    }
    // else if(action == GLFW_RELEASE) {
    //     EventBus::publish(MouseButtonReleaseEvent(code));
    // }
}

// =============================================================================
void TargetWindow::_window_size_callback([[maybe_unused]] GLFWwindow* window,
                                         [[maybe_unused]] int width,
                                         [[maybe_unused]] int height)
{
    EventBus::publish(WindowSizeEvent({
        .width  = static_cast<uint32_t>(width),
        .height = static_cast<uint32_t>(height)
    }));
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