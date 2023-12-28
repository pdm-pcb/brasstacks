#ifndef BRASSTACKS_PLATFORM_VULKAN_VULKAN_FORMATTERS_HPP
#define BRASSTACKS_PLATFORM_VULKAN_VULKAN_FORMATTERS_HPP

#include "brasstacks/pch.hpp"

template <> struct std::formatter<vk::DebugUtilsMessengerEXT> {
    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::DebugUtilsMessengerEXT const &debug,
                std::format_context &ctx) const
    {
        return std::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkDebugUtilsMessengerEXT(debug))
        );
    }
};

template <> struct std::formatter<vk::Device> {
    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::Device const &device, std::format_context &ctx) const {
        return std::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkDevice(device))
        );
    }
};

template <> struct std::formatter<vk::DeviceMemory> {
    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::DeviceMemory const &memory, std::format_context &ctx) const
    {
        return std::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkDeviceMemory(memory))
        );
    }
};

template <> struct std::formatter<vk::Queue> {
    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::Queue const &queue, std::format_context &ctx) const {
        return std::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkQueue(queue))
        );
    }
};

template <> struct std::formatter<vk::CommandPool> {
    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::CommandPool const &pool, std::format_context &ctx) const {
        return std::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkCommandPool(pool))
        );
    }
};

template <> struct std::formatter<vk::CommandBuffer> {
    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::CommandBuffer const &cmd_buffer,
                std::format_context &ctx) const
    {
        return std::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkCommandBuffer(cmd_buffer))
        );
    }
};

template <> struct std::formatter<vk::SurfaceKHR> {
    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::SurfaceKHR const &surface, std::format_context &ctx) const {
        return std::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkSurfaceKHR(surface))
        );
    }
};

template <> struct std::formatter<vk::SwapchainKHR> {
    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::SwapchainKHR const &swapchain,
                std::format_context &ctx) const
    {
        return std::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkSwapchainKHR(swapchain))
        );
    }
};

template <> struct std::formatter<vk::Fence> {
    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::Fence const &fence, std::format_context &ctx) const
    {
        return std::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkFence(fence))
        );
    }
};

template <> struct std::formatter<vk::Semaphore> {
    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::Semaphore const &sem, std::format_context &ctx) const
    {
        return std::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkSemaphore(sem))
        );
    }
};

template <> struct std::formatter<vk::Buffer> {
    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::Buffer const &buffer, std::format_context &ctx) const {
        return std::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkBuffer(buffer))
        );
    }
};

template <> struct std::formatter<vk::Image> {
    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::Image const &image, std::format_context &ctx) const {
        return std::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkImage(image))
        );
    }
};

template <> struct std::formatter<vk::ImageView> {
    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::ImageView const &view, std::format_context &ctx) const {
        return std::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkImageView(view))
        );
    }
};

template <> struct std::formatter<vk::Sampler> {
    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::Sampler const &sampler, std::format_context &ctx) const {
        return std::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkSampler(sampler))
        );
    }
};

template <> struct std::formatter<vk::DescriptorPool> {
    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::DescriptorPool const &pool, std::format_context &ctx) const
    {
        return std::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkDescriptorPool(pool))
        );
    }
};

template <> struct std::formatter<vk::DescriptorSetLayout> {
    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::DescriptorSetLayout const &layout,
                std::format_context &ctx) const
    {
        return std::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkDescriptorSetLayout(layout))
        );
    }
};

template <> struct std::formatter<vk::DescriptorSet> {
    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::DescriptorSet const &set, std::format_context &ctx) const {
        return std::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkDescriptorSet(set))
        );
    }
};

template <> struct std::formatter<vk::RenderPass> {
    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::RenderPass const &render_pass, std::format_context &ctx)
    const {
        return std::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkRenderPass(render_pass))
        );
    }
};

template <> struct std::formatter<vk::Pipeline> {
    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::Pipeline const &pipeline, std::format_context &ctx) const {
        return std::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkPipeline(pipeline))
        );
    }
};

template <> struct std::formatter<vk::PipelineLayout> {
    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::PipelineLayout const &layout,
                std::format_context &ctx) const
    {
        return std::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkPipelineLayout(layout))
        );
    }
};

template <> struct std::formatter<vk::ShaderModule> {
    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::ShaderModule const &shader, std::format_context &ctx) const
    {
        return std::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkShaderModule(shader))
        );
    }
};

template <> struct std::formatter<vk::Framebuffer> {
    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::Framebuffer const &framebuffer,
                std::format_context &ctx) const
    {
        return std::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkFramebuffer(framebuffer))
        );
    }
};

#endif // BRASSTACKS_PLATFORM_VULKAN_VULKAN_FORMATTERS_HPP