#include "brasstacks/brasstacks.hpp"
#include "brasstacks/core/Renderer.hpp"

#include "brasstacks/core/TargetWindow.hpp"
#include "brasstacks/platform/vulkan/vkInstance.hpp"
#include "brasstacks/platform/vulkan/swapchain/vkSurface.hpp"
#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdBuffer.hpp"
#include "brasstacks/platform/vulkan/devices/vkQueue.hpp"
#include "brasstacks/platform/vulkan/descriptors/vkDescriptorPool.hpp"
#include "brasstacks/platform/vulkan/rendering/vkColorDepth.hpp"

namespace btx {

Application *Renderer::_application { nullptr };

vkSurface *Renderer::_surface { nullptr };
vkDevice   Renderer::_device  { };

vkDescriptorPool *Renderer::_descriptor_pool { nullptr };

vkSwapchain Renderer::_swapchain { };
std::vector<vkFrameSync> Renderer::_frame_sync;
uint32_t Renderer::_image_index { std::numeric_limits<uint32_t>::max() };

vkColorDepth *Renderer::_color_depth { nullptr };

// =============================================================================
void Renderer::init(Application *const application) {
    _application = application;

    vkInstance::create();
    _create_surface();
    _select_physical_device();
    _create_device();
}

// =============================================================================
void Renderer::shutdown() {
    _device.destroy();

    vkPhysicalDevice::clear_device_list();

    _surface->destroy();
    delete _surface;
    _surface = nullptr;

    vkInstance::destroy();
}

// =============================================================================
void Renderer::wait_device_idle() {
    _device.wait_idle();
}

// =============================================================================
void Renderer::_create_surface() {
    if(_surface != nullptr) {
        BTX_CRITICAL("Surface already created");
        return;
    }

#ifdef BTX_LINUX
    vk::XlibSurfaceCreateInfoKHR const create_info {
        .pNext = nullptr,
        .flags = { },
        .dpy = TargetWindow::display(),
        .window = TargetWindow::native()
    };
#elif BTX_WINDOWS
    vk::Win32SurfaceCreateInfoKHR const create_info {
        .pNext = nullptr,
        .flags = { },
        .hinstance = nullptr,
        .hwnd = TargetWindow::native()
    };
#endif // BTX platform

    _surface = new vkSurface;
    _surface->create(create_info);
}

// =============================================================================
void Renderer::_select_physical_device() {
    if(!_surface->native()) {
        BTX_CRITICAL("Cannot select physical device without surface.");
        return;
    }

    auto features13 = vk::PhysicalDeviceVulkan13Features {
        .pNext = nullptr,
        // .synchronization2 = VK_TRUE,
        // .dynamicRendering = VK_TRUE,
    };

    auto features12 = vk::PhysicalDeviceVulkan12Features {
        .pNext = &features13,
        // These two features are required for some validation layer features
        // that I can't figure out how to disable. They're probably worth it,
        // though?
        .uniformAndStorageBuffer8BitAccess = VK_TRUE,
        .bufferDeviceAddress = VK_TRUE,
    };

    auto features11 = vk::PhysicalDeviceVulkan11Features {
        .pNext = &features12,
    };

    auto features = vk::PhysicalDeviceFeatures2 {
        .pNext = &features11,
        .features {
            .fillModeNonSolid = VK_TRUE,
            .samplerAnisotropy = VK_TRUE,
        }
    };

    std::vector<char const *> const extensions {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    vkPhysicalDevice::populate_device_list(*_surface, features, extensions);
}

// =============================================================================
void Renderer::_create_device() {
    if(_device.native()) {
        BTX_ERROR("Logical device already created.");
        return;
    }

    _device.create();
}

// =============================================================================
void Renderer::_create_swapchain() {
    if(_swapchain.native()) {
        BTX_CRITICAL("Swapchain already created.");
        return;
    }

    _swapchain.create(*_surface);
    _image_index = std::numeric_limits<uint32_t>::max();
}

// =============================================================================
void Renderer::_destroy_swapchain() {
    _swapchain.destroy();
}

// =============================================================================
void Renderer::_create_frame_sync() {
    auto const image_count = _swapchain.images().size();
    if(_frame_sync.size() != image_count) {
        _destroy_frame_sync();
        _frame_sync.clear();
        _frame_sync.reserve(image_count);

        // Fill in the frame sync structs with default in-place construction
        std::generate_n(
            std::back_inserter(_frame_sync),
            _frame_sync.capacity(),
            []() {
                return vkFrameSync { };
            }
        );
    }

    for(auto &sync_struct : _frame_sync) {
        sync_struct.create_sync_primitives();
        sync_struct.create_cmd_structures();
    }
}

// =============================================================================
void Renderer::_destroy_frame_sync() {
    for(auto &sync_struct : _frame_sync) {
        sync_struct.destroy_sync_primitives();
        sync_struct.destroy_cmd_structures();
    }
}

} // namespace btx
