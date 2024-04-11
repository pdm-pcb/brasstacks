cmake_minimum_required(VERSION 3.25) # Required for FetchContent SYSTEM

project(
    brasstacks
    VERSION 0.1.0
    DESCRIPTION "A Vulkan renderer."
    LANGUAGES CXX C
)

# Where we keep the helpful extra CMake
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# Helper functions
include("add_compiler_definitions")
include("add_compiler_options")
include("add_warning_flags")
include("add_debug_flags")
include("add_release_flags")
include("compile_shaders")

# The library itself
add_subdirectory(third_party)
add_subdirectory(brasstacks)

# If we're not pulled in by something else, go ahead and build a demo
if(PROJECT_IS_TOP_LEVEL)
    add_subdirectory(demo)
endif()
