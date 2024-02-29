include(FetchContent)

function(fetch_glm)
    # set(FETCHCONTENT_QUIET OFF)
    FetchContent_Declare(
        glm SYSTEM
        GIT_REPOSITORY https://github.com/g-truc/glm
        GIT_TAG bf71a834948186f4097caa076cd2663c69a10e1e # 2020-04-13
        # GIT_PROGRESS TRUE
        FIND_PACKAGE_ARGS 0.9.9.8
    )

    set(BUILD_STATIC_LIBS "ON" CACHE BOOL "" FORCE)

    message(STATUS "${PROJECT_NAME}: Fetching glm...")
    FetchContent_MakeAvailable(glm)
endfunction()
