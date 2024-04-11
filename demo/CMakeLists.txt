project(brasstacks_demo)

add_executable(${PROJECT_NAME})

file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE headers "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")

target_sources(${PROJECT_NAME} PRIVATE ${sources} ${headers})

target_link_libraries(${PROJECT_NAME} PRIVATE brasstacks)

file(
    GLOB shader_sources
    "${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders/*.vert"
    "${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders/*.frag"
)

include("compile_shaders")
compile_shader(${PROJECT_NAME} "${shader_sources}")
