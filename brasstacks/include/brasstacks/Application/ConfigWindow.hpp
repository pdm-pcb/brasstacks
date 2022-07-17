#ifndef BRASSTACKS_APPLICATION_CONFIGWINDOW_HPP
#define BRASSTACKS_APPLICATION_CONFIGWINDOW_HPP

#include "brasstacks/Events/EventPublisher.hpp"

namespace btx {

class ConfigWindow : public EventPublisher {
public:
    struct DisplaySettings {
        std::uint16_t x_res;
        std::uint16_t y_res;
        std::uint16_t refresh_rate;

        DisplaySettings(std::uint16_t x, std::uint16_t y, std::uint16_t refresh) :
            x_res { x },
            y_res { y },
            refresh_rate { refresh }
        { }

        DisplaySettings() = delete;
    };

    virtual void run() = 0;

    static ConfigWindow * create();

    virtual ~ConfigWindow() = default;

    ConfigWindow(const ConfigWindow &&) = delete;
    ConfigWindow(ConfigWindow &)        = delete;

    ConfigWindow& operator=(const ConfigWindow &&) = delete;
    ConfigWindow& operator=(ConfigWindow &)        = delete;

protected:
    ConfigWindow() = default;
};

} // namespace btx

#endif // BRASSTACKS_APPLICATION_CONFIGWINDOW_HPP