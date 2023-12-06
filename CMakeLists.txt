cmake_minimum_required(VERSION 3.25) # Required for FetchContent SYSTEM

project(
    brasstacks
    VERSION 0.0.0
    DESCRIPTION "A Vulkan renderer."
    LANGUAGES CXX
)

find_package(Vulkan 1.3 REQUIRED COMPONENTS glslc)

add_subdirectory(btx)

# Only touch tests and demos when we're not being pulled in by something else
if(PROJECT_IS_TOP_LEVEL)
    add_subdirectory(demo)
    add_subdirectory(unit_tests)
endif()