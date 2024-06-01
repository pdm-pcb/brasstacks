include(FetchContent)

function(fetch_SPIRVReflect)
    # set(FETCHCONTENT_QUIET OFF)
    FetchContent_Declare(
        SPIRVReflect SYSTEM
        GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Reflect/
        GIT_TAG ee5b57fba6a986381f998567761bbc064428e645 # 2024-04-27
        # GIT_PROGRESS TRUE
    )

    set(SPIRV_REFLECT_EXECUTABLE     "OFF" CACHE BOOL "" FORCE)
    set(SPIRV_REFLECT_STATIC_LIB     "ON"  CACHE BOOL "" FORCE)
    set(SPIRV_REFLECT_BUILD_TESTS    "OFF" CACHE BOOL "" FORCE)
    set(SPIRV_REFLECT_ENABLE_ASSERTS "ON"  CACHE BOOL "" FORCE)
    set(SPIRV_REFLECT_ENABLE_ASAN    "OFF" CACHE BOOL "" FORCE)

    message(STATUS "${CMAKE_PROJECT_NAME}: Fetching SPIRVReflect...")
    FetchContent_MakeAvailable(SPIRVReflect)
endfunction()
