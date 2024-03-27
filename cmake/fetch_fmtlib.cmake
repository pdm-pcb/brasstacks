include(FetchContent)

function(fetch_fmtlib)
    # set(FETCHCONTENT_QUIET OFF)
    FetchContent_Declare(
        fmtlib SYSTEM
        GIT_REPOSITORY https://github.com/fmtlib/fmt/
        GIT_TAG e69e5f977d458f2650bb346dadf2ad30c5320281 # 2024-01-04
        # GIT_PROGRESS TRUE
        FIND_PACKAGE_ARGS 10.2.1
    )

    message(STATUS "${PROJECT_NAME}: Fetching fmtlib...")
    FetchContent_MakeAvailable(fmtlib)
endfunction()