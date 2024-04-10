include(FetchContent)

function(fetch_spdlog)
    # set(FETCHCONTENT_QUIET OFF)
    FetchContent_Declare(
        spdlog SYSTEM
        GIT_REPOSITORY https://github.com/gabime/spdlog
        GIT_TAG 7c02e204c92545f869e2f04edaab1f19fe8b19fd # 2024-01-12
        # GIT_PROGRESS TRUE
        FIND_PACKAGE_ARGS 1.11.0
    )

    # Sure hope this can work reliably some day
    # set(SPDLOG_USE_STD_FORMAT "ON" CACHE BOOL "" FORCE)

    # Control the version of fmt being used
    set(SPDLOG_FMT_EXTERNAL "ON" CACHE BOOL "" FORCE)

    message(STATUS "${PROJECT_NAME}: Fetching spdlog...")
    FetchContent_MakeAvailable(spdlog)
endfunction()
