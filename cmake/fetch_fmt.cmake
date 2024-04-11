include(FetchContent)

function(fetch_fmt)
    # set(FETCHCONTENT_QUIET OFF)
    FetchContent_Declare(
        fmt SYSTEM
        GIT_REPOSITORY https://github.com/fmtlib/fmt/
        GIT_TAG a33701196adfad74917046096bf5a2aa0ab0bb50
        # GIT_PROGRESS TRUE
        FIND_PACKAGE_ARGS 9.1
    )

    message(STATUS "${PROJECT_NAME}: Fetching fmt...")
    FetchContent_MakeAvailable(fmt)
endfunction()
