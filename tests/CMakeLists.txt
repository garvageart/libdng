find_package(Check)
include_directories(${CHECK_INCLUDE_DIRS})
link_directories(${CHECK_LIBRARY_DIRS})

add_executable(check_mode check_mode.c greatest.h)
target_include_directories(check_mode PUBLIC include)
target_link_libraries(check_mode PUBLIC libdng check)

add_test(NAME check_mode COMMAND check_mode)