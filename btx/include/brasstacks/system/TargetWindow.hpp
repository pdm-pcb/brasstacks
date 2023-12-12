#ifndef BRASSTACKS_SYSTEM_TARGETWINDOW_HPP
#define BRASSTACKS_SYSTEM_TARGETWINDOW_HPP

#include "brasstacks/pch.hpp"

namespace btx {
class TargetWindow {
public:
    static TargetWindow * create(std::string_view const app_name);

    struct Dimensions {
        uint32_t width = 0;
        uint32_t height = 0;
    };

    struct Position {
        int32_t x = 0;
        int32_t y = 0;
    };

    virtual void create_window(Dimensions const &dimensions = { 0, 0 },
                               Position const &position = { 0, 0 }) = 0;
    virtual void show_window() = 0;
    virtual void destroy_window() = 0;

    virtual void create_surface(vk::Instance const &instance) = 0;
    virtual void destroy_surface() = 0;

    virtual void message_loop() = 0;

    virtual vk::SurfaceKHR const & surface() const = 0;

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