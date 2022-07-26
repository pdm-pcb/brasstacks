include(FetchContent)
message(NOTICE "Fetch Freetype2")
FetchContent_Declare(
    freetype
    GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype
    GIT_TAG        e8ebfe988b5f57bfb9a3ecb13c70d9791bce9ecf # 2.12.1 | 2022-05-01
)
FetchContent_MakeAvailable(freetype)

set(FT_DISABLE_ZLIB     TRUE CACHE BOOL "" FORCE)
set(FT_DISABLE_BZIP2    TRUE CACHE BOOL "" FORCE)
set(FT_DISABLE_PNG      TRUE CACHE BOOL "" FORCE)
set(FT_DISABLE_HARFBUZZ TRUE CACHE BOOL "" FORCE)
set(FT_DISABLE_BROTLI   TRUE CACHE BOOL "" FORCE)

set_target_properties(
    freetype PROPERTIES

    INTERFACE_SYSTEM_INCLUDE_DIRECTORIES
    $<TARGET_PROPERTY:freetype,INTERFACE_INCLUDE_DIRECTORIES>
)