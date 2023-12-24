cmake_minimum_required(VERSION 3.25) # Required for FetchContent SYSTEM

project(
    brasstacks
    VERSION 0.0.1
    DESCRIPTION "A Vulkan renderer."
    LANGUAGES CXX C
)

find_package(Vulkan 1.2 REQUIRED COMPONENTS glslc)

add_subdirectory(btx)
add_subdirectory(third_party)

# Only touch tests and demos when we're not being pulled in by something else
if(PROJECT_IS_TOP_LEVEL)
    add_subdirectory(demo)
    add_subdirectory(unit_tests)
endif()