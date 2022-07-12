# top level cmake file
cmake_minimum_required(VERSION 3.16)

# grab the glad sources and library from here, since glad2 requires a special
# command, and cmake doesn't like it when I put this in Findglad.cmake
if("${BTX_RENDER_API}" MATCHES "GL")
    include(FetchContent)

    message(NOTICE "Fetch glad2")
    FetchContent_Declare(
        glad
        GIT_REPOSITORY https://github.com/Dav1dde/glad.git
        GIT_TAG        glad2
    )
    FetchContent_MakeAvailable(glad)

    add_subdirectory("${CMAKE_BINARY_DIR}/_deps/glad-src/cmake")

    glad_add_library(
        libglad STATIC
        LOADER
        LOCATION "${CMAKE_BINARY_DIR}/_deps/glad-build"
        API gl:core=4.6
        API wgl=
    )
endif()

project(
    brasstacks
    VERSION 0.0.1
    DESCRIPTION "A platform for exploring real-time rendering."
    LANGUAGES C CXX
)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    add_compile_definitions("DEBUG")
endif()

add_subdirectory(brasstacks)
add_subdirectory(demo)