set(TARGET_NAME third_party)

add_library(${TARGET_NAME} STATIC)

file(GLOB_RECURSE LIBRARY_SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE LIBRARY_HEADERS
	"${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")

target_sources(
    ${TARGET_NAME}
    PRIVATE
        ${LIBRARY_SOURCES}
		${LIBRARY_HEADERS}
)

target_include_directories(
    ${TARGET_NAME} SYSTEM PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include/"
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR
   CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(${TARGET_NAME} PRIVATE "-w")

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

    target_compile_options(${TARGET_NAME} PRIVATE "/w")

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
        )

        target_link_options(
            ${TARGET_NAME} PRIVATE
            "/OPT:NOREF"    # Remove unused functions
        )
    endif()
endif()