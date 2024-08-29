add_executable(tests tests.cpp)

target_link_libraries(
    tests
    PRIVATE
        brasstacks::brasstacks
        Catch2::Catch2WithMain
)

list(APPEND CMAKE_MODULE_PATH "${Catch2_SOURCE_DIR}/extras")

include(CTest)
include(Catch)

catch_discover_tests(tests)