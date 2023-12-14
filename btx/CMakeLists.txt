set(TARGET_NAME "brasstacks")

add_library(${TARGET_NAME} STATIC)
add_library(brasstacks::brasstacks ALIAS brasstacks)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/version.hpp.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/brasstacks/version.hpp"
)

file(GLOB_RECURSE PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE PROJECT_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")

target_sources(
    ${TARGET_NAME}
    PRIVATE
        ${PROJECT_SOURCES}
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include/"
        FILES ${PROJECT_HEADERS}
)

target_precompile_headers(
    ${TARGET_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include/brasstacks/pch.hpp"
)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/FetchDeps.cmake")
FetchDeps()

target_link_libraries(
    ${TARGET_NAME} PUBLIC
    spdlog::spdlog
    Vulkan::Vulkan
)

if(LINUX)
    target_compile_definitions(
        ${TARGET_NAME} PRIVATE
        BTX_LINUX
    )
elseif(WIN32)
    target_compile_definitions(
        ${TARGET_NAME} PRIVATE
        BTX_WINDOWS
    )
endif()

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    target_compile_definitions(
        ${TARGET_NAME} PRIVATE
        BTX_DEBUG
    )
elseif(CMAKE_BUILD_TYPE MATCHES "Release")
    target_compile_definitions(
        ${TARGET_NAME} PRIVATE
        BTX_RELEASE
    )
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR
   CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(
        ${TARGET_NAME} PRIVATE
        "-Wall"         # Enable most warnings
        "-Wextra"
        "-pedantic"     # and language compliance checks
    )

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(
            ${TARGET_NAME} PRIVATE
            "-Weverything"      # Clang supports enabling all possible warnings
            "-Wno-c++98-compat" # ...which means some must then be disabled
            "-Wno-c++98-compat-pedantic"
            "-Wno-c++20-compat" # We're targeting C++20
            "-Wno-newline-eof"
            "-fsafe-buffer-usage-suggestions"
        )
    endif()

    if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|amd64")
        target_compile_options(
            ${TARGET_NAME} PRIVATE
            "-march=x86-64-v3"    # Target Haswell/Excavator or newer
            "-mtune=x86-64-v3"
        )
    endif()

    if(CMAKE_BUILD_TYPE MATCHES "Debug")
        target_compile_options(
            ${TARGET_NAME} PRIVATE
            "-O0"       # No optimization
            "-ggdb3"    # All the gdb flags
        )
    elseif(CMAKE_BUILD_TYPE MATCHES "Release")
        target_compile_options(
            ${TARGET_NAME} PRIVATE
            "-O3"       # Optimize for speed, sacrifice size
        )
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_definitions(
        ${TARGET_NAME} PRIVATE
        _CRT_SECURE_NO_WARNINGS # Do not warn about C string functions
    )

    target_compile_options(
        ${TARGET_NAME} PRIVATE
        "/Wall"         # All warnings not disabled by default
        "/wd4820"       # Ignore warning for padding
        "/wd5045"       # Ignore warning for spectre mitigation
    )

    if(CMAKE_SYSTEM_PROCESSOR MATCHES "AMD64")
        target_compile_options(
            ${TARGET_NAME} PRIVATE
            "/arch:AVX2"    # Target Haswell/Excavator or newer
        )
    endif()

    if(CMAKE_BUILD_TYPE MATCHES "Debug")
        target_compile_options(
            ${TARGET_NAME} PRIVATE
            "/Od"       # Optimization disabled
            "/Zi"		# Generate .pdb files: implies /debug
            "/JMC"      # Just-My-Code: only step over user code
            "/RTCs"     # Runtime checks for stack frame behaviors
            "/RTCu"     # Runtime checks for uninitialized variables
            "/GS"       # Buffer overrun checks
            "/sdl"      # Compile and runtime security checks
            "/diagnostics:column" # Include column in compiler messages
        )

        target_link_options(
            ${TARGET_NAME} PRIVATE
            "/DEBUG:FULL" # Move private symbol information into single .pdb
        )
    elseif(CMAKE_BUILD_TYPE MATCHES "Release")
        target_compile_options(
            ${TARGET_NAME} PRIVATE
            "/O2"       # Optimize for speed. Implies:
                        # - global optimization
                        # - intrinsics
                        # - "favor fast code"
                        # - frame/pointer omission
                        # - inline function expansion
                        # - eliminate duplicate strings
                        # - function-level linking
            "/Ob3"      # More aggressive inline expansions than /O2
        )

        target_link_options(
            ${TARGET_NAME} PRIVATE
            "/OPT:NOREF"    # Remove unused functions
        )
    endif()
endif()

set_target_properties(
	${TARGET_NAME} PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF

	ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/debug/lib"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/debug/lib"

	ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/release/lib"
	LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/release/lib"
)