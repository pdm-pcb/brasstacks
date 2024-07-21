#include "brasstacks/brasstacks.hpp"
#include "brasstacks/core/TargetWindow.hpp"

#include "brasstacks/platform/input/GLFWToBTXKeys.hpp"

namespace btx {
GLFWwindow *TargetWindow::_window { nullptr };

Offset TargetWindow::_screen_center   { };
Size   TargetWindow::_window_size     { };
Offset TargetWindow::_window_position { };

// =============================================================================
void TargetWindow::init(std::string_view const app_name) {
    if(::glfwInit() == 0) {
        BTX_CRITICAL("Failed to initialize GLFW");
        return;
    }

    BTX_INFO("Initialized GLFW {:s}", ::glfwGetVersionString());

    ::glfwSetErrorCallback(TargetWindow::_error_callback);

    ::glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    ::glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    // ::glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

    _window = ::glfwCreateWindow(
        320, 240,        // Default size that'll change immediately
        app_name.data(), // Window title/class/etc
        nullptr,         // Windowed mode
        nullptr          // No shared resources
    );

    if(_window == nullptr) {
        BTX_CRITICAL("GLFW window creation failed");
        return;
    }

    if(::glfwRawMouseMotionSupported() != GLFW_TRUE) {
        BTX_CRITICAL("Raw mouse input not supported by this platform.");
        return;
    }

    ::glfwSetKeyCallback(_window, TargetWindow::_key_callback);

    _get_resolution();
    _size_and_place();
}

// =============================================================================
void TargetWindow::shutdown() {
    ::glfwDestroyWindow(_window);
    ::glfwTerminate();
}

// =============================================================================
bool TargetWindow::poll_events() {
    ::glfwPollEvents();
    return static_cast<bool>(::glfwWindowShouldClose(_window));
}

// =============================================================================
void TargetWindow::_size_and_place() {
    ::glfwSetWindowSize(_window,
                        static_cast<int>(_window_size.width),
                        static_cast<int>(_window_size.height));

    auto const half_width  = static_cast<float>(_window_size.width)  * 0.5f;
    auto const half_height = static_cast<float>(_window_size.height) * 0.5f;

    _window_position = {
        .x = (static_cast<uint32_t>(_screen_center.x)
              - static_cast<uint32_t>(half_width)),
        .y = (static_cast<uint32_t>(_screen_center.y)
              - static_cast<uint32_t>(half_height)),
    };

    ::glfwSetWindowPos(_window, _window_position.x, _window_position.y);
}

// =============================================================================
void TargetWindow::_get_resolution() {
    auto const *current_mode = ::glfwGetVideoMode(::glfwGetPrimaryMonitor());

    auto const width = static_cast<float>(current_mode->width);
    auto const height = static_cast<float>(current_mode->height);

    _window_size = {
        .width  = static_cast<int32_t>(width * 0.75f),
        .height = static_cast<int32_t>(height * 0.75f)
    };

    _screen_center = {
        .x = static_cast<uint32_t>(width * 0.5f),
        .y = static_cast<uint32_t>(height * 0.5f)
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
{ }

// =============================================================================
void TargetWindow::_mouse_move_callback([[maybe_unused]] GLFWwindow* window,
                                        [[maybe_unused]] double x,
                                        [[maybe_unused]] double y)
{ }

// =============================================================================
void TargetWindow::_mouse_button_callback([[maybe_unused]] GLFWwindow* window,
                                          [[maybe_unused]] int button,
                                          [[maybe_unused]] int action,
                                          [[maybe_unused]] int mods)
{ }

// =============================================================================
void TargetWindow::_window_iconify_callback([[maybe_unused]] GLFWwindow* window,
                                            [[maybe_unused]] int iconified)
{ }

} // namespace btx