add_executable(
    demo
	"src/demo.cpp"
    "src/load_resources.cpp"
    "src/add_terrain.cpp"
    "src/add_cube.cpp"
    "src/add_camera.cpp"
    "src/CubeSystem.cpp"
    "src/CameraSystem.cpp"
)

target_include_directories(
	demo PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(
	demo PUBLIC
	brasstacks
)

set_target_properties(
	demo PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED ON
	RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/debug/bin
	RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/release/bin
)