#ifndef BRASSTACKS_SYSTEM_TARGETWINDOW_HPP
#define BRASSTACKS_SYSTEM_TARGETWINDOW_HPP

#include "brasstacks/pch.hpp"

namespace btx {
class TargetWindow {
public:
    struct Dimensions final {
        std::uint32_t width = 0;
        std::uint32_t height = 0;
    };

    struct Position final {
        std::int32_t x = 0;
        std::int32_t y = 0;
    };

    static TargetWindow * create(std::string_view const app_name,
                                 Dimensions const &dimensions = { 0u, 0u},
                                 Position const &position = { 0, 0 });

    virtual void show_window() = 0;
    virtual void hide_window() = 0;
    virtual void message_loop() = 0;

#if defined(BTX_LINUX)

    // ...!

#elif defined(BTX_WINDOWS)

    virtual ::HWND const & native() const = 0;

#endif // BTX platform

    virtual ~TargetWindow() = default;

    TargetWindow(TargetWindow &&) = delete;
    TargetWindow(const TargetWindow &) = delete;

    TargetWindow & operator=(TargetWindow &&) = delete;
    TargetWindow & operator=(const TargetWindow &) = delete;

protected:
    TargetWindow() = default;
};

} // namespace btx

#endif // BRASSTACKS_SYSTEM_WINDOW_TARGETWINDOW_HPP