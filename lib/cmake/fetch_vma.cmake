include(FetchContent)

function(fetch_vma)
    # set(FETCHCONTENT_QUIET OFF)
    FetchContent_Declare(
        vma SYSTEM
        GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
        GIT_TAG a6bfc237255a6bac1513f7c1ebde6d8aed6b5191 # 2022-05-26
        # GIT_PROGRESS TRUE
        FIND_PACKAGE_ARGS 3.0.1
    )

    message(STATUS "${PROJECT_NAME}: Fetching vma...")
    FetchContent_MakeAvailable(vma)
endfunction()
