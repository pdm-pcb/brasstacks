# No warnings for third party software
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options("-w")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options("/w")
endif()

# stb --------------------------------------------------------------------------
add_subdirectory(stb)
target_include_directories(stb SYSTEM PUBLIC "${CMAKE_CURRENT_LIST_DIR}")