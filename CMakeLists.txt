cmake_minimum_required(VERSION 3.25) # Required for FetchContent SYSTEM

project(
    brasstacks
    VERSION 0.1.0
    DESCRIPTION "A Vulkan renderer."
    LANGUAGES CXX C
)

# Everything depends on Vulkan
set(target_vulkan_version 1.2)
find_package(Vulkan ${target_vulkan_version})

# Unless otherwise specified, stick to the standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Where we keep the helpful extra CMake
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# Helper functions
include("set_target_architecture")
include("add_compiler_definitions")
include("add_warning_flags")
include("add_debug_flags")
include("add_release_flags")
include("compile_shaders")

set_target_architecture()

# Third party dependencies
include("fetch_fmt")
include("fetch_spdlog")
include("fetch_glfw3")
include("fetch_glm")
include("fetch_VulkanMemoryAllocator")

fetch_fmt()
fetch_spdlog()
fetch_glfw3()
# fetch_glm()
fetch_VulkanMemoryAllocator()

add_subdirectory(brasstacks)
add_subdirectory(third_party)

# Only touch tests and demos when we're not being pulled in by something else
if(PROJECT_IS_TOP_LEVEL)
    add_subdirectory(demo)
    add_subdirectory(unit_tests)

    include(GNUInstallDirs)
    install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            COMPONENT ${PROJECT_NAME}_runtime
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT ${PROJECT_NAME}_runtime
            NAMELINK_COMPONENT ${PROJECT_NAME}_development
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT ${PROJECT_NAME}_development
    )

    include(InstallRequiredSystemLibraries)
    set(CPACK_PACKAGE_VERSION ${CMAKE_PROJECT_VERSION})
    set(CPACK_SOURCE_GENERATOR "ZIP")
    include(CPack)
endif()
