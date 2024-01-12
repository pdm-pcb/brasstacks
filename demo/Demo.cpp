#include "Demo.hpp"

#include "brasstacks/platform/vulkan/devices/vkCmdBuffer.hpp"
#include "brasstacks/platform/vulkan/rendering/vkSwapchain.hpp"

#include "brasstacks/config/RenderConfig.hpp"
#include "brasstacks/rendering/meshes/PlaneMesh.hpp"
#include "brasstacks/rendering/passes/ColorDepthPass.hpp"

Demo::Demo() :
    Application("Demo"),
    _device { nullptr },
    _swapchain { nullptr },
    _color_depth_pass { nullptr }
{ }

void Demo::init(btx::vkDevice const &device, btx::vkSwapchain const &swapchain)
{
    _device = &device;
    create_swapchain_resources(swapchain);
}

void Demo::shutdown() {
    destroy_swapchain_resources();
}

void Demo::update() {

}

void Demo::record_commands(btx::vkCmdBuffer const &cmd_buffer,
                           uint32_t const image_index)
{
    _color_depth_pass->begin(cmd_buffer, image_index);
    // send push constants
    // bind descriptor sets
    // draw indexed
    // etc
    _color_depth_pass->end();
}

void Demo::destroy_swapchain_resources() {
    delete _color_depth_pass;
}

void Demo::create_swapchain_resources(btx::vkSwapchain const &swapchain) {
    _swapchain = &swapchain;
    _color_depth_pass = new btx::ColorDepthPass(*_device, *_swapchain);
}