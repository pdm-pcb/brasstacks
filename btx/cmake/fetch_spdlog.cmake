include(FetchContent)

function(fetch_spdlog)
    # set(FETCHCONTENT_QUIET OFF)
    FetchContent_Declare(
        spdlog SYSTEM
        GIT_REPOSITORY https://github.com/gabime/spdlog
        GIT_TAG 7e635fca68d014934b4af8a1cf874f63989352b7 # 2023-07-08
        # GIT_PROGRESS TRUE
        FIND_PACKAGE_ARGS 1.12.0
    )

    # Have spdlog to use std::format since C++20 support is better nowadays
    set(SPDLOG_USE_STD_FORMAT "ON" CACHE BOOL "" FORCE)

    message(STATUS "${PROJECT_NAME}: Fetching spdlog...")
    FetchContent_MakeAvailable(spdlog)
endfunction()