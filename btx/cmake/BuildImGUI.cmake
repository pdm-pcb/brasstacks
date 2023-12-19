# ==============================================================================

function(BuildImGUI)
    file(GLOB_RECURSE IMGUI_HEADER ${CMAKE_SOURCE_DIR}/*imgui.h)
    cmake_path(GET IMGUI_HEADER PARENT_PATH IMGUI_PATH)
    message("${IMGUI_PATH}")

    message("Building Dear ImGUI from ${IMGUI_PATH}")

    set(TARGET_NAME imgui)

    add_library(${TARGET_NAME} STATIC)

    target_sources(
        ${TARGET_NAME} PRIVATE
        "${IMGUI_PATH}/imgui.cpp"
        "${IMGUI_PATH}/imgui_demo.cpp"
        "${IMGUI_PATH}/imgui_draw.cpp"
        "${IMGUI_PATH}/imgui_tables.cpp"
        "${IMGUI_PATH}/imgui_widgets.cpp"
        "${IMGUI_PATH}/backends/imgui_impl_vulkan.cpp"
        "${IMGUI_PATH}/backends/imgui_impl_win32.cpp"
    )

    target_include_directories(
        ${TARGET_NAME} SYSTEM PUBLIC
        "${IMGUI_PATH}"
        "${IMGUI_PATH}/backends"
    )

    target_link_libraries(${TARGET_NAME} PRIVATE Vulkan::Vulkan)

    set_target_properties(
        ${TARGET_NAME} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
    )

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR
       CMAKE_CXX_COMPILER_ID MATCHES "Clang")
     target_compile_options(${TARGET_NAME} PRIVATE "-w")

    elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        target_compile_options(${TARGET_NAME} PRIVATE "/w")
    endif()

endfunction()