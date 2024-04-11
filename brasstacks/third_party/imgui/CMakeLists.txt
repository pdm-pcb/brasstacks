add_library(
    imgui STATIC EXCLUDE_FROM_ALL
    "imgui_demo.cpp"
    "imgui_draw.cpp"
    "imgui_impl_glfw.cpp"
    "imgui_impl_vulkan.cpp"
    "imgui_tables.cpp"
    "imgui_widgets.cpp"
    "imgui.cpp"
)

target_link_libraries(
    imgui PRIVATE
        glfw
        Vulkan::Vulkan
        GPUOpen::VulkanMemoryAllocator
)