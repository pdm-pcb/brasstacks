#include "brasstacks/system/Application.hpp"

#include "brasstacks/events/EventBroker.hpp"
#include "brasstacks/events/keyboard_events.hpp"
#include "brasstacks/system/window/TargetWindow.hpp"

#include "brasstacks/platform/vulkan/vkInstance.hpp"
#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkLogicalDevice.hpp"

#include "brasstacks/platform/vulkan/vkSwapchain.hpp"

namespace btx {

// =============================================================================
void Application::run() {
    TargetWindow::show_window();

    this->init();

    while(_running) {
        TargetWindow::message_loop();
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

    TargetWindow::init(app_name);
    TargetWindow::create_window();

    _graphics_api = new vkInstance();

    TargetWindow::create_surface(*_graphics_api);

    _adapter = new vkPhysicalDevice(
        *_graphics_api,
        {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME
        },
        {
            vkPhysicalDevice::Features::SAMPLER_ANISOTROPY,
            vkPhysicalDevice::Features::FILL_MODE_NONSOLID,
        }
    );

    _device = new vkLogicalDevice(
        *_adapter,
        {
            #ifdef BTX_DEBUG
            "VK_LAYER_KHRONOS_validation",
            #endif // BTX_DEBUG
        }
    );

    _swapchain = new vkSwapchain(*_adapter, *_device);
}

Application::~Application() {
    delete _swapchain;
    delete _device;

    TargetWindow::destroy_surface(*_graphics_api);
    TargetWindow::destroy_window();
    TargetWindow::shutdown();

    delete _adapter;
    delete _graphics_api;

    EventBroker::shutdown();
}

} // namespace btx