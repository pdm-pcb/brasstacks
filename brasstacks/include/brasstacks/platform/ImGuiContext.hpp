#ifndef BRASSTACKS_PLATFORM_IMGUICONTEXT_HPP
#define BRASSTACKS_PLATFORM_IMGUICONTEXT_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/platform/vulkan/descriptors/vkDescriptorPool.hpp"

namespace btx {

class vkColorDepthPass;

class ImGuiContext {
public:
    static void init_window(::GLFWwindow *window);
    static void create_descriptor_pool();
    static void create_swapchain_resources(vkColorDepthPass const &render_pass);
    static void destroy_swapchain_resources();
    static void destroy_descriptor_pool();
    static void shutdown_window();

    ImGuiContext() = delete;
    ~ImGuiContext() = delete;

    ImGuiContext(ImGuiContext &&) = delete;
    ImGuiContext(ImGuiContext const &) = delete;

    ImGuiContext & operator=(ImGuiContext &&) = delete;
    ImGuiContext & operator=(ImGuiContext const &) = delete;

private:
    static ::ImGuiIO *_io;
    static std::unique_ptr<vkDescriptorPool> _descriptor_pool;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_IMGUICONTEXT_HPP