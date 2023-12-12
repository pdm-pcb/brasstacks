#include "brasstacks/system/Application.hpp"

#include "brasstacks/events/EventBroker.hpp"
#include "brasstacks/events/keyboard_events.hpp"

#include "brasstacks/system/TargetWindow.hpp"

#include "brasstacks/platform/vulkan/vkInstance.hpp"
#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"

#include "brasstacks/platform/vulkan/vkSwapchain.hpp"

namespace btx {

// =============================================================================
void Application::run() {
    _target_window->show_window();

    this->init();

    while(_running) {
        _target_window->message_loop();
    }

    this->shutdown();
}

// =============================================================================
void Application::on_key_release(KeyReleaseEvent const &event) {
    if(event.code == BTX_KB_ESCAPE) {
        _running = false;
    }
}

// =============================================================================
Application::Application(std::string_view const app_name) :
    _running { true }
{
    ConsoleLog::init();

    EventBroker::init();
    EventBroker::subscribe<KeyReleaseEvent>(
        this,
        &Application::on_key_release
    );

    _target_window = TargetWindow::create(app_name);
    _target_window->create_window();

    _graphics_api = new vkInstance();

    _target_window->create_surface(_graphics_api->native());

    _adapter = new vkPhysicalDevice(
        *_graphics_api,
        _target_window->surface(),
        {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME
        },
        {
            vkPhysicalDevice::Features::SAMPLER_ANISOTROPY,
            vkPhysicalDevice::Features::FILL_MODE_NONSOLID,
        }
    );

    _device = new vkDevice(
        *_adapter
#ifdef BTX_DEBUG
        , { "VK_LAYER_KHRONOS_validation", }
#endif // BTX_DEBUG
    );

    _swapchain = new vkSwapchain(
        _adapter->native(),
        _device->native(),
        _target_window->surface()
    );
}

Application::~Application() {
    delete _swapchain;
    delete _device;

    _target_window->destroy_surface();
    _target_window->destroy_window();
    delete _target_window;

    delete _adapter;
    delete _graphics_api;

    EventBroker::shutdown();
}

} // namespace btx