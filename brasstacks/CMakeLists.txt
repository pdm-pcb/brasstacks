set(target_name "brasstacks")

# Everything depends on Vulkan
set(VK_TARGET_VERSION 1.2)
find_package(Vulkan ${VK_TARGET_VERSION} REQUIRED)

# Third party dependencies
include("fetch_fmt")
include("fetch_spdlog")
include("fetch_glfw3")
include("fetch_VulkanMemoryAllocator")

fetch_fmt()
fetch_spdlog()
fetch_glfw3()
fetch_VulkanMemoryAllocator()

# The library itself
add_library(${target_name} STATIC)
add_library(brasstacks::brasstacks ALIAS brasstacks)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/version.hpp.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/brasstacks/version.hpp"
)

target_precompile_headers(
    ${target_name} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include/brasstacks/pch.hpp"
)

file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE headers "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")

target_sources(
    ${target_name}
    PRIVATE ${sources}
    PUBLIC ${headers}
)

target_link_libraries(
    ${target_name}
    PUBLIC
        fmt
        spdlog::spdlog
        glfw
        Vulkan::Vulkan
        GPUOpen::VulkanMemoryAllocator
    PRIVATE
        third_party
)

target_include_directories(
    ${target_name}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${STB_INCLUDE_DIR}
)

set_target_properties(
    ${target_name} PROPERTIES
        SOVERSION ${CMAKE_PROJECT_VERSION_MAJOR}
        VERSION ${CMAKE_PROJECT_VERSION}
        DEBUG_POSTFIX "d"
)

add_compiler_definitions(${target_name})
add_compiler_options(${target_name})
add_warning_flags(${target_name})
add_debug_flags(${target_name})
add_release_flags(${target_name})
