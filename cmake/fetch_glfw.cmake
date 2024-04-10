include(FetchContent)

function(fetch_glfw)
    # set(FETCHCONTENT_QUIET OFF)
    FetchContent_Declare(
        glfw SYSTEM
        GIT_REPOSITORY https://github.com/glfw/glfw/
        GIT_TAG 7b6aead9fb88b3623e3b3725ebb42670cbe4c579 # 2024-02-23
        # GIT_PROGRESS TRUE
        FIND_PACKAGE_ARGS 3.4
    )

    set(BUILD_SHARED_LIBS   "OFF" CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES "OFF" CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS    "OFF" CACHE BOOL "" FORCE)
    set(GLFW_BUILD_DOCS     "OFF" CACHE BOOL "" FORCE)
    set(GLFW_INSTALL        "OFF" CACHE BOOL "" FORCE)
    set(GLFW_VULKAN_STATIC  "ON"  CACHE BOOL "" FORCE)

    message(STATUS "${PROJECT_NAME}: Fetching glfw...")
    FetchContent_MakeAvailable(glfw)
endfunction()
