// #include "brasstacks/brasstacks.hpp"
// #include "brasstacks/rendering/Renderer.hpp"

// #include "brasstacks/system/TargetWindow.hpp"
// #include "brasstacks/config/RenderConfig.hpp"


// #include "brasstacks/platform/vulkan/vkInstance.hpp"
// #include "brasstacks/platform/vulkan/rendering/vkSurface.hpp"
// #include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"
// #include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
// #include "brasstacks/platform/vulkan/devices/vkQueue.hpp"
// #include "brasstacks/platform/vulkan/devices/vkCmdBuffer.hpp"
// #include "brasstacks/platform/vulkan/rendering/vkSwapchain.hpp"
// #include "brasstacks/platform/vulkan/rendering/vkFrameSync.hpp"

// #include "brasstacks/system/UILayer.hpp"

// namespace btx {

// // =============================================================================
// Renderer::Renderer(TargetWindow &target_window) :
//     _target_window      { target_window },
//     _device             { nullptr },
//     _surface            { nullptr },
//     _swapchain          { nullptr },
//     _image_acquire_sems { },
//     _frame_sync         { },
//     _image_index        { std::numeric_limits<uint32_t>::max() },
//     _ui_layer           { nullptr }
// {
//     vkInstance::create();

//     // First, acquire the details necessary to construct a Vulkan surface from
//     // the target window
//     _create_surface();

//     // Then use the information from that surface to choose a physical device
//     _select_physical_device();

//     // Instantiate a logical device based on the physical device
//     _create_device();

//     // Create the swapchain using the logical device and surface
//     _create_swapchain();

//     // And create the semaphores and vkFrameSync structures to manage the
//     // swapchain images
//     _create_frame_sync();

//     // Finally, enable the UI overlay
//     _create_ui_layer();
// }

// // =============================================================================
// Renderer::~Renderer() {
//     destroy_swapchain_and_resources();
//     _destroy_device();
// }

// // =============================================================================
// uint32_t Renderer::acquire_next_image() {
//     if(_image_acquire_sems.empty()) {
//         BTX_CRITICAL("Swapchain ran out of image acquire semaphores.");
//         return std::numeric_limits<uint32_t>::max();
//     }

//     // Grab the first available semaphore
//     auto const acquire_sem = _image_acquire_sems.front();
//     _image_acquire_sems.pop();

//     // And ask the swapchain which index comes next
//     _image_index = _swapchain->acquire_image_index(acquire_sem);

//     // Something has gone wrong with the swapchain
//     if(_image_index == std::numeric_limits<uint32_t>::max()) {
//         // Put it back
//         _image_acquire_sems.push(acquire_sem);
//         return _image_index;
//     }

//     // Wrap the corresponding frame's data for convenience
//     auto &frame = *_frame_sync[_image_index];

//     // Wait on this frame's queue fence, which should always be signaled by
//     // the time we get here. After waiting, reset the submit fence and
//     // command pool.
//     frame.wait_and_reset();

//     // Now swap this frame's image acquire semaphore out for the one we just
//     // submitted
//     if(frame.image_acquire_semaphore()) {
//         _image_acquire_sems.push(frame.image_acquire_semaphore());
//     }

//     frame.image_acquire_semaphore() = acquire_sem;

//     return _image_index;
// }

// // =============================================================================
// vkCmdBuffer const & Renderer::begin_recording() {
//     auto &frame_sync  = *_frame_sync[_image_index];
//     auto const &cmd_buffer  = frame_sync.cmd_buffer();

//     cmd_buffer.begin_one_time_submit();

//     return cmd_buffer;
// }

// // =============================================================================
// void Renderer::end_recording() {
//     auto &frame_sync  = *_frame_sync[_image_index];
//     auto const &cmd_buffer  = frame_sync.cmd_buffer();

//     // _ui_layer->render_pass(cmd_buffer, _image_index);

//     cmd_buffer.end_recording();
// }

// // =============================================================================
// void Renderer::submit_commands() {
//     auto &frame_sync = *_frame_sync[_image_index];

//     static vk::PipelineStageFlags const wait_stage {
//         vk::PipelineStageFlagBits::eColorAttachmentOutput
//     };

//     vk::SubmitInfo const submit_info {
//         .pNext                = nullptr,
//         .waitSemaphoreCount   = 1u,
//         .pWaitSemaphores      = &frame_sync.image_acquire_semaphore(),
//         .pWaitDstStageMask    = &wait_stage,
//         .commandBufferCount   = 1u,
//         .pCommandBuffers      = &frame_sync.cmd_buffer().native(),
//         .signalSemaphoreCount = 1u,
//         .pSignalSemaphores    = &frame_sync.cmds_complete_semaphore(),
//     };

//     auto const result = _device->graphics_queue().native().submit(
//         1u,
//         &submit_info,
//         frame_sync.queue_fence()
//     );

//     if(result != vk::Result::eSuccess) {
//         BTX_CRITICAL("Failed to submit commands to device queue: '{}'",
//                      vk::to_string(result));
//     }
// }

// // =============================================================================
// bool Renderer::present_image() {
//     auto &frame_sync = *_frame_sync[_image_index];
//     return _swapchain->present(frame_sync, _image_index);
// }

// // =============================================================================
// void Renderer::wait_device_idle() {
//     _device->wait_idle();
// }

// // =============================================================================
// void Renderer::create_swapchain_and_resources() {
//     _create_surface();
//     _create_swapchain();
//     _create_frame_sync();
//     _create_ui_layer();
// }

// // =============================================================================
// void Renderer::destroy_swapchain_and_resources() {
//     _destroy_ui_layer();
//     _destroy_frame_sync();
//     _destroy_swapchain();
//     _destroy_surface();
// }

// // =============================================================================
// void Renderer::_create_surface() {
// #if defined(BTX_LINUX)

//     vk::XlibSurfaceCreateInfoKHR const create_info {
//       .pNext = nullptr,
//       .flags = { },
//       .dpy = nullptr,
//       .window = { }
//     };

// #elif defined(BTX_WINDOWS)

//     vk::Win32SurfaceCreateInfoKHR const create_info {
//       .pNext = nullptr,
//       .flags = { },
//       .hinstance = nullptr,
//       .hwnd = _target_window.native()
//     };

// #endif // BTX platform

//     _surface = new vkSurface(create_info);
// }

// // =============================================================================
// void Renderer::_select_physical_device() {
//     vkPhysicalDevice::select(
//         *_surface,
//         {
//             vkPhysicalDevice::Features::SAMPLER_ANISOTROPY,
//             vkPhysicalDevice::Features::FILL_MODE_NONSOLID,
//         },
//         {
//             VK_KHR_SWAPCHAIN_EXTENSION_NAME,
//         }
//     );
// }

// // =============================================================================
// void Renderer::_create_device() {
//     _device = new vkDevice(
// #ifdef BTX_DEBUG
//         { "VK_LAYER_KHRONOS_validation", }
// #endif // BTX_DEBUG
//     );
// }

// // =============================================================================
// void Renderer::_create_swapchain() {
//     _swapchain = new vkSwapchain(*_device, *_surface);
// }

// // =============================================================================
// void Renderer::_create_frame_sync() {
//     for(uint32_t i = 0; i < RenderConfig::swapchain_image_count; ++i) {
//         _frame_sync.push_back(new vkFrameSync(*_device));
//     }

//     // Create one extra semaphore because by the time the first n frames have
//     // requested images from the swapchain, the queue of semaphores will be
//     // empty. And since the logic in acquire_next_image() tries to get a new
//     // semaphore before releasing the old one for that frame, there has to be
//     // one semaphore spare.
//     auto result = _device->native().createSemaphore({ });
//     if(result.result != vk::Result::eSuccess) {
//         BTX_CRITICAL("Unable to create semaphore: '{}'",
//                         vk::to_string(result.result));
//         return;
//     }

//     BTX_TRACE("Created image acquire semaphore {}", result.value);
//     _image_acquire_sems.push(result.value);
// }

// // =============================================================================
// void Renderer::_create_ui_layer() {
//     // _ui_layer = new UILayer(*_device, _target_window, *_swapchain);
// }

// // =============================================================================
// void Renderer::_destroy_surface() {
//     delete _surface;
//     _surface = nullptr;
// }

// // =============================================================================
// void Renderer::_destroy_device() {
//     delete _device;
//     _device = nullptr;
// }

// // =============================================================================
// void Renderer::_destroy_swapchain() {
//     delete _swapchain;
//     _swapchain = nullptr;
// }

// // =============================================================================
// void Renderer::_destroy_frame_sync() {
//     for(auto *frame : _frame_sync) {
//         delete frame;
//     }

//     _frame_sync.clear();

//     while(!_image_acquire_sems.empty()) {
//         auto const sem = _image_acquire_sems.front();

//         BTX_TRACE("Destroying image acquire semaphore {}", sem);
//         _device->native().destroySemaphore(sem);

//         _image_acquire_sems.pop();
//     }
// }

// // =============================================================================
// void Renderer::_destroy_ui_layer() {
//     delete _ui_layer;
//     _ui_layer = nullptr;
// }

// } // namespace btx