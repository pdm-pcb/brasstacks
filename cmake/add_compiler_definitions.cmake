function(add_compiler_definitions target_name)
    if(CMAKE_BUILD_TYPE MATCHES "Debug")
        target_compile_definitions(
            ${target_name} PRIVATE
            BTX_DEBUG
        )
    elseif(CMAKE_BUILD_TYPE MATCHES "Release")
        target_compile_definitions(
            ${target_name} PRIVATE
            BTX_RELEASE
        )
    endif()

    if(LINUX)
        target_compile_definitions(
            ${target_name} PUBLIC
            BTX_LINUX
        )
    elseif(WIN32)
        target_compile_definitions(
            ${target_name} PUBLIC
            BTX_WINDOWS
            _CRT_SECURE_NO_WARNINGS # Do not warn about C string functions
        )
    endif()
endfunction()