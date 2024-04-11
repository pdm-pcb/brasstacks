# The library itself
add_library(${PROJECT_NAME} STATIC)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

configure_file("version.hpp.in" "version.hpp")

target_precompile_headers(${PROJECT_NAME} PUBLIC "pch.hpp")

file(GLOB_RECURSE sources "*.cpp")
file(GLOB_RECURSE headers "*.hpp")

target_sources(${PROJECT_NAME} PRIVATE ${sources} PUBLIC ${headers})

target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC
        fmt
        spdlog::spdlog
        glfw
        Vulkan::Vulkan
        GPUOpen::VulkanMemoryAllocator
    PRIVATE
        stb
        imgui
)
