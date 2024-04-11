function(add_compiler_definitions)
    if(CMAKE_BUILD_TYPE MATCHES "Debug")
        add_compile_definitions(BTX_DEBUG)
    elseif(CMAKE_BUILD_TYPE MATCHES "Release")
        add_compile_definitions(BTX_RELEASE)
    endif()

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        add_compile_definitions(BTX_GNU)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_compile_definitions(BTX_CLANG)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        add_compile_definitions(BTX_MSVC)
    endif()

    if(LINUX)
        add_compile_definitions(BTX_LINUX)
    elseif(WIN32)
        add_compile_definitions(
            BTX_WINDOWS
            # Since I disabled compiler extensions, a lot of Microsoft's stdlib
            # gets upset
            _SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS
        )
    endif()
endfunction()