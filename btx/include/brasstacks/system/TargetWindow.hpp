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

    void set_overlay_input(bool const state) { _overlay_input = state; }

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
    TargetWindow() : _overlay_input { false } { }

    bool _overlay_input_enabled() const { return _overlay_input; }

private:
    // Controls whether or not the UI overlay should receive input messages
    bool _overlay_input;
};

} // namespace btx

#endif // BRASSTACKS_SYSTEM_WINDOW_TARGETWINDOW_HPP