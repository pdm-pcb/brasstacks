add_library(brasstacks STATIC)
add_library(brasstacks::brasstacks ALIAS brasstacks)

set(BTX_ROOT "${CMAKE_CURRENT_LIST_DIR}")
add_subdirectory(src)