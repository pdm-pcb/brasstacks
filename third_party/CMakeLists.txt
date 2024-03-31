set(target_name third_party)

add_library(${target_name} STATIC)

file(GLOB_RECURSE sources
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE headers
	"${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")

target_sources(
    ${target_name}
    PRIVATE
        ${sources}
		${headers}
)

target_include_directories(
    ${target_name} SYSTEM PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include/"
)

target_link_libraries(
    ${target_name} PUBLIC
    Vulkan::Vulkan
    glfw
)

add_debug_flags(${target_name})
add_release_flags(${target_name})

# Disable warnings for third party code
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR
   CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(${target_name} PRIVATE "-w")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(${target_name} PRIVATE "/w")
endif()