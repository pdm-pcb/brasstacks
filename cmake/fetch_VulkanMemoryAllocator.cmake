include(FetchContent)

function(fetch_VulkanMemoryAllocator)
    # set(FETCHCONTENT_QUIET OFF)
    FetchContent_Declare(
        VulkanMemoryAllocator SYSTEM
        GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
        GIT_TAG 19b940e864bd3a5afb3c79e3c6788869d01a19eb # 2024-02-27
        # GIT_PROGRESS TRUE
    )

    # brasstacks uses Vulkan-Hpp's dynamic loader
    set(VMA_DYNAMIC_VULKAN_FUNCTIONS "ON"  CACHE BOOL "" FORCE)

    # All other options off
    set(VMA_BUILD_DOCUMENTATION      "OFF" CACHE BOOL "" FORCE)
    set(VMA_BUILD_SAMPLES            "OFF" CACHE BOOL "" FORCE)
    set(VMA_STATIC_VULKAN_FUNCTIONS  "OFF" CACHE BOOL "" FORCE)

    message(STATUS "${CMAKE_PROJECT_NAME}: Fetching VulkanMemoryAllocator...")
    FetchContent_MakeAvailable(VulkanMemoryAllocator)
endfunction()
