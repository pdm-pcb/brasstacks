set(target_name "brasstacks")

add_library(${target_name} STATIC)
add_library(brasstacks::brasstacks ALIAS brasstacks)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/version.hpp.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/brasstacks/version.hpp"
)

file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE headers "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")

target_sources(
    ${target_name}
    PRIVATE
        ${sources}
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include/"
        FILES ${headers}
)

target_precompile_headers(
    ${target_name} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include/brasstacks/pch.hpp"
)

add_compiler_definitions(${target_name})
add_warning_flags(${target_name})
add_debug_flags(${target_name})
add_release_flags(${target_name})

target_link_libraries(
    ${target_name} PUBLIC
    spdlog::spdlog
    glfw
    Vulkan::Vulkan
    # glm::glm_static
    third_party
)

target_include_directories(
    ${target_name} PRIVATE
    ${STB_INCLUDE_DIR}
)
