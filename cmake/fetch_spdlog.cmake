include(FetchContent)

function(fetch_spdlog)
    # set(FETCHCONTENT_QUIET OFF)
    FetchContent_Declare(
        spdlog SYSTEM
        GIT_REPOSITORY https://github.com/gabime/spdlog
        GIT_TAG 76fb40d95455f249bd70824ecfcae7a8f0930fa3
        # GIT_PROGRESS TRUE
        FIND_PACKAGE_ARGS 1.10
    )

    # Sure hope this can work reliably some day
    # set(SPDLOG_USE_STD_FORMAT "ON" CACHE BOOL "" FORCE)

    # Control the version of fmt being used
    set(SPDLOG_FMT_EXTERNAL "ON" CACHE BOOL "" FORCE)

    message(STATUS "${PROJECT_NAME}: Fetching spdlog...")
    FetchContent_MakeAvailable(spdlog)
endfunction()
