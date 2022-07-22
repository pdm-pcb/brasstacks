# top level cmake file
cmake_minimum_required(VERSION 3.16)

project(
    brasstacks
    VERSION 0.0.1
    DESCRIPTION "A platform for exploring real-time rendering."
    LANGUAGES C CXX
)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    add_compile_definitions("DEBUG")
endif()

add_subdirectory(thirdparty)
add_subdirectory(brasstacks)
add_subdirectory(demo)