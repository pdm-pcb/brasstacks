# Third-party software we build
add_subdirectory(third_party)

# The library itself
message(STATUS "Building ${PROJECT_NAME} v${PROJECT_VERSION}")
add_subdirectory(${PROJECT_NAME})