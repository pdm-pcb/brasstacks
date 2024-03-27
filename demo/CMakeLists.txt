set(target_name "brasstacks_demo")

add_executable(${target_name})

file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE headers "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")

target_sources(
    ${target_name} PRIVATE
	${sources}
	${headers}
)

add_compiler_definitions(${target_name})
add_warning_flags(${target_name})
add_debug_flags(${target_name})
add_release_flags(${target_name})

target_link_libraries(
    ${target_name} PRIVATE
    brasstacks
)

file(
    GLOB shader_sources
    "${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders/*.vert"
    "${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders/*.frag"
)

compile_shader(${target_name} "${shader_sources}")

set_target_properties(
	${target_name} PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/debug/bin"
	RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/demo"
)
