#include "brasstacks/system/Application.hpp"

#include "brasstacks/events/EventBroker.hpp"
#include "brasstacks/events/keyboard_events.hpp"
#include "brasstacks/system/window/TargetWindow.hpp"

#include "brasstacks/platform/vulkan/VkInstance.hpp"
#include "brasstacks/platform/vulkan/devices/VkPhysicalDevice.hpp"
#include "brasstacks/platform/vulkan/devices/VkLogicalDevice.hpp"

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

    VkInstance::init();

    TargetWindow::init(app_name);
    TargetWindow::create_window();
    TargetWindow::create_surface();

    VkPhysicalDevice::query_devices(
        {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME
        },
        {
            VkPhysicalDevice::Features::SAMPLER_ANISOTROPY,
            VkPhysicalDevice::Features::FILL_MODE_NONSOLID,
        }
    );
    VkPhysicalDevice::select_device();

    VkLogicalDevice::create({
        #ifdef BTX_DEBUG
            "VK_LAYER_KHRONOS_validation",
        #endif // BTX_DEBUG
    });
}

Application::~Application() {
    VkLogicalDevice::destroy();
    TargetWindow::destroy_surface();
    TargetWindow::destroy_window();
    TargetWindow::shutdown();
    VkInstance::shutdown();
    EventBroker::shutdown();
}

} // namespace btx