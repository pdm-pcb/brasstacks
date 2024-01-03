/**
 * @file TargetWindow.hpp
 * @brief The base class for a window that will be drawn to.
 */

#ifndef BRASSTACKS_SYSTEM_TARGETWINDOW_HPP
#define BRASSTACKS_SYSTEM_TARGETWINDOW_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/config/RenderConfig.hpp"

namespace btx {

/**
 * @brief The base class for a window that will be drawn to.
 *
 * TargetWindow is inherited from by X11TargetWindow, Win32TargetWindow, etc.
 * The static create() function wraps the platform choice in preprocessor
 * conditionals, allowing Application to just request a window agnostically.
 */
class TargetWindow {
public:

    /**
     * @brief Called by Application to instantiate a TargetWindow.
     * @param app_name User-provided name which is applied to window titles,
     * driver hints, and so on.
     * @param dimensions Desired window dimensions. Defaults to 75% of the
     * primary display's non-scaled resolution.
     * @param position Desired window position. Defaults to centered on the
     * primary display.
     * @return TargetWindow*
     */
    static TargetWindow * create(
        std::string_view const app_name,
        RenderConfig::SurfaceDimensions const &dimensions = { 0u, 0u},
        RenderConfig::SurfacePosition const &position = { 0, 0 }
    );

    /**
     * @brief Make the window visible.
     */
    virtual void show_window() = 0;

    /**
     * @brief Stop showing the window.
     */
    virtual void hide_window() = 0;

    /**
     * @brief Run through and process messages from the operating system.
     */
    virtual void message_loop() = 0;

    void set_ui_input(bool const state) { _ui_input = state; }

    bool ui_input_enabled() const { return _ui_input; }

    float dpi_scale() const { return _dpi_scale; }

#if defined(BTX_LINUX)

    // ...!

#elif defined(BTX_WINDOWS)

    /**
     * @brief Return a native win32 window handle.
     * @return ::HWND const &
     *
     * This function is used by Renderer to create a Vulkan surface.
     */
    virtual ::HWND const & native() const = 0;

#endif // BTX platform

    virtual ~TargetWindow() = default;

    TargetWindow(TargetWindow &&) = delete;
    TargetWindow(const TargetWindow &) = delete;

    TargetWindow & operator=(TargetWindow &&) = delete;
    TargetWindow & operator=(const TargetWindow &) = delete;

protected:
    TargetWindow() :
        _ui_input { false },
        _dpi_scale { 1.0f }
    { }

    void _set_dpi_scale(float const scale) { _dpi_scale = scale; }

private:
    // Controls the UI or the simulation should receive input messages
    bool _ui_input;

    float _dpi_scale;
};

} // namespace btx

#endif // BRASSTACKS_SYSTEM_WINDOW_TARGETWINDOW_HPP