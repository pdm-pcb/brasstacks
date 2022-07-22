if("${BTX_RENDER_API}" MATCHES "GL")
    if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
        # ...
    endif()

    if(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
        add_library(
            glad STATIC
            "src/gl.c"
            "src/wgl.c"
        )
    endif()

    target_include_directories(
        glad INTERFACE PUBLIC
        "include/"
    )
endif()

add_library(
    stb STATIC
    "src/stb.c"
)

target_include_directories(
    stb INTERFACE PUBLIC
    "include/"
)

set(thirdparty_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/include/" CACHE STRING "" FORCE)