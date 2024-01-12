#include "brasstacks/brasstacks.hpp"
#include "brasstacks/core/TargetWindow.hpp"

#include "brasstacks/events/EventBus.hpp"
#include "brasstacks/events/window_events.hpp"
#include "brasstacks/events/keyboard_events.hpp"

namespace btx {

static void error_callback(int error, char const *desc) noexcept;

static void key_callback(GLFWwindow *window, int key, int scancode,
                         int action, int mods) noexcept;

static void size_callback(GLFWwindow *window, int width, int height) noexcept;

// =============================================================================
TargetWindow::TargetWindow(std::string_view const app_name) :
    _window  { nullptr }
{
    if(::glfwInit() == 0) {
        BTX_CRITICAL("Failed to initialize GLFW");
        return;
    }

    ::glfwSetErrorCallback(error_callback);

    _set_window_dimensions();

    ::glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    _window = ::glfwCreateWindow(
        static_cast<int>(RenderConfig::target_window_size.width),
        static_cast<int>(RenderConfig::target_window_size.height),
        app_name.data(),
        nullptr,
        nullptr
    );

    ::glfwSetWindowPos(
        _window,
        RenderConfig::target_window_position.x,
        RenderConfig::target_window_position.y
    );

    if(_window == nullptr) {
        BTX_CRITICAL("GLFW window creation failed");
        return;
    }

    ::glfwSetKeyCallback(_window, key_callback);
    ::glfwSetWindowSizeCallback(_window, size_callback);

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
void TargetWindow::_set_window_dimensions() {
    auto const *video_mode = ::glfwGetVideoMode(::glfwGetPrimaryMonitor());

    auto const width  = static_cast<float>(video_mode->width)  * 0.75f;
    auto const height = static_cast<float>(video_mode->height) * 0.75f;

    RenderConfig::target_window_size = {
        .width  = static_cast<uint32_t>(width),
        .height = static_cast<uint32_t>(height),
    };

    auto const center_x = static_cast<float>(video_mode->width) * 0.5f;
    auto const center_y = static_cast<float>(video_mode->height) * 0.5f;

    RenderConfig::target_window_position = {
        .x = static_cast<int32_t>(center_x - (width * 0.5f)),
        .y = static_cast<int32_t>(center_y - (height * 0.5f)),
    };
}

// =============================================================================
void error_callback(int error, char const *desc) noexcept {
    BTX_ERROR("GLFW Error {}: {:s}", error, desc);
}

// =============================================================================
void key_callback([[maybe_unused]] GLFWwindow *window,
                  [[maybe_unused]] int key,
                  [[maybe_unused]] int scancode,
                  [[maybe_unused]] int action,
                  [[maybe_unused]] int mods) noexcept
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
        EventBus::publish(KeyReleaseEvent { .code = BTX_KB_ESCAPE });
    }
}

// =============================================================================
void size_callback([[maybe_unused]] GLFWwindow* window,
                   int width, int height) noexcept
{
    RenderConfig::target_window_size = {
        .width  = static_cast<uint32_t>(width),
        .height = static_cast<uint32_t>(height),
    };
}

} // namespace btx