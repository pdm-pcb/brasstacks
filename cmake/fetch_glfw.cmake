include(FetchContent)

function(fetch_glfw)
    # set(FETCHCONTENT_QUIET OFF)
    FetchContent_Declare(
        glfw SYSTEM
        GIT_REPOSITORY https://github.com/glfw/glfw/
        GIT_TAG e2c92645460f680fd272fd2eed591efb2be7dc31 # 2023-12-13
        # GIT_PROGRESS TRUE
        FIND_PACKAGE_ARGS 3.3.9
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