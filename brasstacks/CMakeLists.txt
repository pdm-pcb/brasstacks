project(brasstacks)

# Third-party software we build
add_subdirectory(third_party)

# The library itself
add_subdirectory(brasstacks)
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}")