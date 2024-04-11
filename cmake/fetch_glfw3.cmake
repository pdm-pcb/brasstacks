include(FetchContent)

function(fetch_glfw3)
    # set(FETCHCONTENT_QUIET OFF)
    FetchContent_Declare(
        glfw3 SYSTEM
        GIT_REPOSITORY https://github.com/glfw/glfw/
        GIT_TAG 7482de6071d21db77a7236155da44c172a7f6c9e
        # GIT_PROGRESS TRUE
        FIND_PACKAGE_ARGS 3.3.8
    )

    # set(BUILD_SHARED_LIBS   "OFF" CACHE BOOL "" FORCE)
    # set(GLFW_BUILD_EXAMPLES "OFF" CACHE BOOL "" FORCE)
    # set(GLFW_BUILD_TESTS    "OFF" CACHE BOOL "" FORCE)
    # set(GLFW_BUILD_DOCS     "OFF" CACHE BOOL "" FORCE)
    # set(GLFW_INSTALL        "OFF" CACHE BOOL "" FORCE)
    # set(GLFW_VULKAN_STATIC  "ON"  CACHE BOOL "" FORCE)
    # set(GLFW_BUILD_X11      "ON"  CACHE BOOL "" FORCE)
    # set(GLFW_BUILD_WAYLAND  "OFF" CACHE BOOL "" FORCE)

    message(STATUS "${PROJECT_NAME}: Fetching glfw3...")
    FetchContent_MakeAvailable(glfw3)
endfunction()
