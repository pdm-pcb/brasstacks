include(FetchContent)

function(fetch_catch2)
    # set(FETCHCONTENT_QUIET OFF)
    FetchContent_Declare(
        Catch2 SYSTEM
        GIT_REPOSITORY https://github.com/catchorg/Catch2/
        GIT_TAG 31588bb4f56b638dd5afc28d3ebff9b9dcefb88d
        # GIT_PROGRESS TRUE
        FIND_PACKAGE_ARGS 3.7.0
    )

    message(STATUS "${CMAKE_PROJECT_NAME}: Fetching Catch2...")
    FetchContent_MakeAvailable(Catch2)
endfunction()
