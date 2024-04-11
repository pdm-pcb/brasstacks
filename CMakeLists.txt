cmake_minimum_required(VERSION 3.25) # Required for FetchContent SYSTEM

project(
    brasstacks_renderer
    VERSION 0.1.0
    DESCRIPTION "A Vulkan renderer."
    LANGUAGES CXX C
)

# Everything depends on Vulkan
set(BTX_VK_TARGET_VERSION "1.2" CACHE STRING "Target Vulkan version" FORCE)
add_compile_definitions("BTX_VK_TARGET_VERSION=VK_API_VERSION_1_2")
find_package(Vulkan ${BTX_VK_TARGET_VERSION} REQUIRED)

# Where we keep the helpful extra CMake
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# These three functions set flags useful for both first- and third-party
include("set_target_arch")
include("add_debug_flags")
include("add_release_flags")

set_target_arch()   # Target newer CPUs, specifically AVX2
add_debug_flags()   # Disable optimizations and add debugging info
add_release_flags() # Enable optimizations and strip excess symbols

# External dependencies
include("fetch_fmt")
include("fetch_spdlog")
include("fetch_glfw3")
include("fetch_VulkanMemoryAllocator")

fetch_fmt()
fetch_spdlog()
fetch_glfw3()
fetch_VulkanMemoryAllocator()

# Use standard C++20
set(CMAKE_CXX_STANDARD          20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

# Use standard C17
set(CMAKE_C_STANDARD            17)
set(CMAKE_C_STANDARD_REQUIRED   ON)
set(CMAKE_C_EXTENSIONS          OFF)

# These functions make more sense for first-party software
include("add_compiler_definitions")
include("add_warning_flags")

add_compiler_definitions()  # brasstacks specific defines
add_warning_flags()         # Many, many warnings enabled

# The library itself
add_subdirectory(brasstacks)

# If we're not pulled in by something else, go ahead and build a demo
if(PROJECT_IS_TOP_LEVEL)
    add_subdirectory(demo)
endif()
