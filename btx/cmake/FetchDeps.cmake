function(FetchDeps)
    include(FetchContent)

    # ==========================================================================
    message(STATUS "${PROJECT_NAME}: Fetching spdlog...")
    FetchContent_Declare(
        spdlog SYSTEM
        GIT_REPOSITORY https://github.com/gabime/spdlog
        GIT_TAG 7e635fca68d014934b4af8a1cf874f63989352b7 # 2023-07-08
        FIND_PACKAGE_ARGS 1.12.0
    )

    # Have spdlog to use std::format since C++20 support is better nowadays
    set(SPDLOG_USE_STD_FORMAT "ON" CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(spdlog)

    # ==========================================================================
    message(STATUS "${PROJECT_NAME}: Fetching Dear ImGui...")
    FetchContent_Declare(
        imgui SYSTEM
        GIT_REPOSITORY https://github.com/ocornut/imgui
        GIT_TAG b81bd7ed984ce095c20a059dd0f4d527e006998f # 2023-11-15
        FIND_PACKAGE_ARGS 1.90
    )

    FetchContent_MakeAvailable(imgui)
endfunction()