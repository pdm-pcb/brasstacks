set(CMAKE_EXPORT_COMPILE_COMMANDS OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
find_package(spdlog REQUIRED)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(
    brasstacks STATIC
    "src/Utility/Logger.cpp"
	"src/Application/Application.cpp"
    "src/Application/ConfigWindow.cpp"
    "src/Engine/Engine.cpp"
    "src/Engine/RenderConfig.cpp"
    "src/Application/TargetWindow.cpp"
    "src/Engine/RenderContext.cpp"
    "src/Events/WindowEvent.cpp"
    "src/Events/KeyboardEvent.cpp"
    "src/Events/MouseEvent.cpp"
    "src/Events/EventPublisher.cpp"
    "src/Meshes/VertexBuffer.cpp"
    "src/Meshes/VertexLayout.cpp"
    "src/Meshes/IndexBuffer.cpp"
)

target_include_directories(
	brasstacks PUBLIC
	"include/"
)

target_precompile_headers(
    brasstacks PUBLIC
    "include/brasstacks/System/pch.hpp"
)

target_link_libraries(
	brasstacks PUBLIC
	"spdlog"
)

if("${BTX_RENDER_API}" MATCHES "GL")
	target_compile_definitions(
		brasstacks PUBLIC
		"BTX_OPENGL"
	)

    target_link_libraries(
        brasstacks PUBLIC
        "libglad"
    )

    target_include_directories(
        brasstacks PUBLIC
        "${GLAD_DIR}/include"
    )

	if(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
		target_link_libraries(
			brasstacks PUBLIC
			"opengl32.lib"
		)

        target_sources(
            brasstacks PRIVATE
            "src/Platform/GL/GLContextWGL.cpp"
        )
	elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
		target_link_libraries(
			brasstacks PUBLIC
			"GL"
			"GLX"
		)
	endif()
endif()

# -----------------------------------------------------------------------------
# Windows build details
if(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    target_sources(
        brasstacks PRIVATE
        "src/Platform/Win32/Win32ConfigWindow.cpp"
        "src/Platform/Win32/Win32TargetWindow.cpp"
    )

	target_compile_options(
		brasstacks PUBLIC
		"/nologo"  # Suppress copyright message
		"/W4"	   # Warnin's
		"/wd4312"  # Disable the warning for casting to a bigger size
	)

	target_compile_definitions(
		brasstacks PUBLIC
		"_CRT_SECURE_NO_WARNINGS" # ignore "may be unsafe" warnings
		"WIN64"
	)

    target_link_libraries(
        brasstacks PUBLIC
        "Shcore.lib"
    )

	if(CMAKE_BUILD_TYPE MATCHES "Debug")
		target_compile_options(
			brasstacks PUBLIC
			"/JMC"      # Just-My-Code
			"/Zi"		# Debugging information
			"/RTC1"     # Runtime checks
			"/GS"       # Buffer overrun checks
			"/sdl"      # Security warnings
			"/Od"       # Optimization disabled
			"/EHsc"     # Standard exception handling
			"/diagnostics:column" # Include column number in compiler messages
        )

        target_compile_definitions(
            brasstacks PUBLIC
            "DEBUG"
        )
	elseif(CMAKE_BUILD_TYPE MATCHES "Release")
		target_compile_options(
			brasstacks PUBLIC
			"/O2"         # Optimization not disabled =D
			"/fp:fast"    # Gotta go fast
			"/GS-"        # No buffer overrun checks
		)
	endif()
endif()

set_target_properties(
	brasstacks PROPERTIES
	CXX_STANDARD 23
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
    PCH_WARN_INVALID OFF
	ARCHIVE_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/debug/lib
	LIBRARY_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/debug/lib
	ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/release/lib
	LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/release/lib
)

get_target_property(
    brasstacks_flags brasstacks COMPILE_OPTIONS
)

get_target_property(
	brasstacks_defs brasstacks COMPILE_DEFINITIONS
)

get_target_property(
    brasstacks_inc brasstacks INCLUDE_DIRECTORIES
)

message(STATUS "brasstacks compile flags: ${brasstacks_flags}")
message(STATUS "brasstacks compile defines: ${brasstacks_defs}")
message(STATUS "brasstacks include directories: ${brasstacks_inc}")
message(STATUS "cmake compile flags: ${CMAKE_CXX_FLAGS}")
