#ifndef BRASSTACKS_SYSTEM_APPLICATION_HPP
#define BRASSTACKS_SYSTEM_APPLICATION_HPP

#include "brasstacks/pch.hpp"

namespace btx {

struct KeyReleaseEvent;

class TargetWindow;
class vkInstance;
class vkPhysicalDevice;
class vkDevice;
class vkSwapchain;

class Application {
public:
    virtual void init()     = 0;
    virtual void shutdown() = 0;

    void run();

    void on_key_release(KeyReleaseEvent const &event);

    explicit Application(std::string_view const app_name);
    virtual ~Application();

    Application() = delete;

    Application(Application &&) = delete;
    Application(const Application &) = delete;

    Application & operator=(Application &&) = delete;
    Application & operator=(const Application &) = delete;

private:
    bool _running;

    TargetWindow       *_target_window;
    vkInstance         *_graphics_api;
    vkPhysicalDevice   *_adapter;
    vkDevice           *_device;
    vkSwapchain        *_swapchain;
};

} // namespace btx

#endif // BRASSTACKS_SYSTEM_APPLICATION_HPP