include(FetchContent)

function(fetch_fmt)
    # set(FETCHCONTENT_QUIET OFF)
    FetchContent_Declare(
        fmt SYSTEM
        GIT_REPOSITORY https://github.com/fmtlib/fmt/
        GIT_TAG e69e5f977d458f2650bb346dadf2ad30c5320281 # 2024-01-04 v10.2.1
        # GIT_PROGRESS TRUE
        FIND_PACKAGE_ARGS 9.1
    )

    message(STATUS "${PROJECT_NAME}: Fetching fmt...")
    FetchContent_MakeAvailable(fmt)
endfunction()
