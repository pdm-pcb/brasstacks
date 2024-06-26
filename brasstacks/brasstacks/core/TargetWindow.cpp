#include "brasstacks/brasstacks.hpp"
#include "brasstacks/core/TargetWindow.hpp"

#include "brasstacks/events/EventBus.hpp"
#include "brasstacks/events/window_events.hpp"
#include "brasstacks/events/keyboard_events.hpp"
#include "brasstacks/events/mouse_events.hpp"
#include "brasstacks/platform/input/GLFWToBTXKeys.hpp"

#include "brasstacks/platform/imgui/UIOverlay.hpp"

namespace btx {
GLFWwindow *TargetWindow::_window { nullptr };

RenderConfig::Size   TargetWindow::_screen_size     { };
RenderConfig::Offset TargetWindow::_screen_center   { };
RenderConfig::Size   TargetWindow::_window_size     { };
RenderConfig::Offset TargetWindow::_window_position { };

double TargetWindow::_last_cursor_x { 0.0 };
double TargetWindow::_last_cursor_y { 0.0 };

bool TargetWindow::_mouse_captured { false };

// =============================================================================
void TargetWindow::init(std::string_view const app_name) {
    if(::glfwInit() == 0) {
        BTX_CRITICAL("Failed to initialize GLFW");
        return;
    }

    BTX_INFO("Initialized GLFW {:s}", ::glfwGetVersionString());

    ::glfwSetErrorCallback(TargetWindow::_error_callback);

    _get_resolutions();

    ::glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    ::glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    ::glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

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

    // Default to one notch down from a full screen resolution
    RenderConfig::current_resolution = &RenderConfig::available_resolutions[1];
    RenderConfig::current_resolution->selected = true;
    size_and_place();

    ::glfwSetKeyCallback(_window, TargetWindow::_key_callback);
    ::glfwSetMouseButtonCallback(_window, TargetWindow::_mouse_button_callback);
    ::glfwSetCursorPosCallback(_window, _mouse_move_callback);
    ::glfwSetWindowIconifyCallback(_window,
                                   TargetWindow::_window_iconify_callback);

    UIOverlay::init_window(_window, app_name);
}

// =============================================================================
void TargetWindow::shutdown() {
    UIOverlay::shutdown_window();

    ::glfwDestroyWindow(_window);
    ::glfwTerminate();
}

// =============================================================================
void TargetWindow::capture_mouse() {
    ::glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    ::glfwSetInputMode(_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    ::glfwGetCursorPos(_window, &_last_cursor_x, &_last_cursor_y);

    _mouse_captured = true;
    UIOverlay::set_enabled(false);
}

// =============================================================================
void TargetWindow::release_mouse() {
    ::glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    ::glfwSetInputMode(_window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);

    _mouse_captured = false;
    UIOverlay::set_enabled(true);
}

// =============================================================================
void TargetWindow::poll_events() {
    ::glfwPollEvents();

    if(::glfwWindowShouldClose(_window)) {
        BTX_TRACE("TargetWindow publishing window close event");
        EventBus::publish(WindowEvent(WindowEventType::WINDOW_CLOSE));
    }
}

// =============================================================================
void TargetWindow::size_and_place(RenderConfig::Size const &size) {
    if(size.width != 0u && size.height != 0u) {
        _window_size = size;
    }
    else {
        _window_size = RenderConfig::current_resolution->size;
    }
    ::glfwSetWindowSize(_window,
                        static_cast<int>(_window_size.width),
                        static_cast<int>(_window_size.height));

    // ::glfwSetWindowMonitor(
    //     _window,
    //     ::glfwGetPrimaryMonitor(),
    //     0, 0,
    //     static_cast<int>(_window_size.width),
    //     static_cast<int>(_window_size.height),
    //     static_cast<int>(RenderConfig::current_resolution->refresh_rate)
    // );

    auto const half_width  = static_cast<float>(_window_size.width)  * 0.5f;
    auto const half_height = static_cast<float>(_window_size.height) * 0.5f;

    _window_position = {
        .x = (static_cast<int32_t>(_screen_center.x)
              - static_cast<int32_t>(half_width)),
        .y = (static_cast<int32_t>(_screen_center.y)
              - static_cast<int32_t>(half_height)),
    };

    ::glfwSetWindowPos(_window, _window_position.x, _window_position.y);
}

// =============================================================================
float TargetWindow::scale_factor() {
    float x;
    float y;

    ::glfwGetWindowContentScale(_window, &x, &y);

    if(std::abs(x - y) >= 0.000001f) {
        BTX_WARN("GLFW reported scaling factor x: {:.06f}, y: {:.06f}; "
                 "returning larger of the two.", x, y);
    }

    return std::max(x, y);
}

// =============================================================================
void TargetWindow::_get_resolutions() {
    auto const *current_mode = ::glfwGetVideoMode(::glfwGetPrimaryMonitor());

    size_t mode_count = 0;
    auto const* available_modes = ::glfwGetVideoModes(
        ::glfwGetPrimaryMonitor(),
        reinterpret_cast<int *>(&mode_count)
    );

    RenderConfig::available_resolutions.reserve(mode_count);

    BTX_INFO("Found {} display modes:", mode_count);

    uint64_t prev_pixel_count = 0u;

    for(size_t mode_index = mode_count - 1u; mode_index > 0u; --mode_index) {
        auto const &mode = available_modes[mode_index];

        auto const current_pixel_count =
            static_cast<uint64_t>(mode.width * mode.height);

        if(current_pixel_count == prev_pixel_count) {
            continue;
        }

        prev_pixel_count = current_pixel_count;

        BTX_INFO("  {}x{}@{}", mode.width, mode.height, mode.refreshRate);

        RenderConfig::available_resolutions.emplace_back(
            RenderConfig::SelectedResolution {
                .size = {
                    .width = static_cast<uint32_t>(mode.width),
                    .height = static_cast<uint32_t>(mode.height),
                },
                // Might come back to this later
                // .refresh_rate = static_cast<uint32_t>(mode.refreshRate),
                .selected = false,
        });
    }

    // Grab some temporaries for populating fractional sizes
    auto const width = static_cast<float>(current_mode->width);
    auto const height = static_cast<float>(current_mode->height);

    _screen_center = {
        .x = static_cast<int32_t>(width * 0.5f),
        .y = static_cast<int32_t>(height * 0.5f)
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
    if(!_mouse_captured && UIOverlay::key_callback_handled()) {
        return;
    }

    auto const code = GLFWToBTXKeys::translate(key);
    if(action == GLFW_PRESS) {
        EventBus::publish(KeyboardEvent(KeyboardEventType::KEY_PRESS, code));
    }
    else if(action == GLFW_RELEASE) {
        EventBus::publish(KeyboardEvent(KeyboardEventType::KEY_RELEASE, code));
    }
}

// =============================================================================
void TargetWindow::_mouse_move_callback([[maybe_unused]] GLFWwindow* window,
                                        [[maybe_unused]] double x,
                                        [[maybe_unused]] double y)
{
    static double x_offset = 0.0;
    static double y_offset = 0.0;

    x_offset = x - _last_cursor_x;
    y_offset = y - _last_cursor_y;

    _last_cursor_x = x;
    _last_cursor_y = y;

    EventBus::publish(
        MouseMoveEvent(static_cast<int32_t>(x_offset),
                       static_cast<int32_t>(y_offset))
    );
}

// =============================================================================
void TargetWindow::_mouse_button_callback([[maybe_unused]] GLFWwindow* window,
                                          [[maybe_unused]] int button,
                                          [[maybe_unused]] int action,
                                          [[maybe_unused]] int mods)
{
    if(!_mouse_captured && UIOverlay::mouse_button_callback_handled()) {
        return;
    }

    auto const code = GLFWToBTXKeys::translate(button);

    if(action == GLFW_PRESS) {
        EventBus::publish(
            MouseButtonEvent(MouseButtonEventType::BUTTON_PRESS, code)
        );
    }
    else if(action == GLFW_RELEASE) {
        EventBus::publish(
            MouseButtonEvent(MouseButtonEventType::BUTTON_RELEASE, code)
        );
    }
}

// =============================================================================
void TargetWindow::_window_iconify_callback([[maybe_unused]] GLFWwindow* window,
                                            [[maybe_unused]] int iconified)
{
    if(iconified == GLFW_TRUE) {
        BTX_TRACE("TargetWindow publishing window minimize event");
        EventBus::publish(WindowEvent(WindowEventType::WINDOW_MINIMIZE));
    }
    else {
        BTX_TRACE("TargetWindow publishing window restore event");
        EventBus::publish(WindowEvent(WindowEventType::WINDOW_RESTORE));
    }
}

} // namespace btx