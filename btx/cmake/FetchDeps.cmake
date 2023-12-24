function(FetchDeps)
    include(FetchContent)
    # set(FETCHCONTENT_QUIET OFF)

    # ==========================================================================
    message(STATUS "${PROJECT_NAME}: Fetching spdlog...")
    FetchContent_Declare(
        spdlog SYSTEM
        GIT_REPOSITORY https://github.com/gabime/spdlog
        GIT_TAG 7e635fca68d014934b4af8a1cf874f63989352b7 # 2023-07-08
        # GIT_PROGRESS TRUE
        FIND_PACKAGE_ARGS 1.12.0
    )
    # Have spdlog to use std::format since C++20 support is better nowadays
    set(SPDLOG_USE_STD_FORMAT "ON" CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(spdlog)

    # ==========================================================================
    # message(STATUS "${PROJECT_NAME}: Fetching glm...")
    # FetchContent_Declare(
    #     glm SYSTEM
    #     GIT_REPOSITORY https://github.com/g-truc/glm/
    #     GIT_TAG bf71a834948186f4097caa076cd2663c69a10e1e # 2020-04-13
    #     GIT_PROGRESS TRUE
    #     FIND_PACKAGE_ARGS 0.9.9.8
    # )
    # FetchContent_MakeAvailable(glm)

    # ==========================================================================
    # message(STATUS "${PROJECT_NAME}: Fetching stb...")
    # FetchContent_Declare(
    #     stb SYSTEM
    #     GIT_REPOSITORY https://github.com/nothings/stb
    #     GIT_PROGRESS TRUE
    # )
    # FetchContent_MakeAvailable(stb)

    # file(GLOB_RECURSE
    #      STB_FETCH_DIR
    #      "${CMAKE_SOURCE_DIR}/build/**/stb_image.h")

    # cmake_path(GET STB_FETCH_DIR PARENT_PATH STB_SRC_DIR)

    # set(STB_INCLUDE_DIR ${STB_SRC_DIR} CACHE PATH "" FORCE)

endfunction()