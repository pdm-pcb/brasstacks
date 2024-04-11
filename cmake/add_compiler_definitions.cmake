function(add_compiler_definitions target_name)
    target_compile_definitions(
        ${target_name} PRIVATE
        "VK_TARGET_VERSION=VK_API_VERSION_1_2"
    )

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

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        target_compile_definitions(${target_name} PUBLIC BTX_GNU)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_definitions(${target_name} PUBLIC BTX_CLANG)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        target_compile_definitions(${target_name} PUBLIC BTX_MSVC)
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