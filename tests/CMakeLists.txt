find_package(Check)
include_directories(${CHECK_INCLUDE_DIRS})
link_directories(${CHECK_LIBRARY_DIRS})

add_executable(check_mode check_mode.c greatest.h)
target_include_directories(check_mode PUBLIC include)
target_link_libraries(check_mode PUBLIC libdng check)

add_executable(check_dng check_dng.c greatest.h)
target_include_directories(check_dng PUBLIC include)
target_link_libraries(check_dng PUBLIC libdng check)

add_executable(check_dcp check_dcp.c greatest.h)
target_include_directories(check_dcp PUBLIC include)
target_link_libraries(check_dcp PUBLIC libdng check)
configure_file(fixture/pinephone.dcp ${CMAKE_CURRENT_BINARY_DIR}/fixture/pinephone.dcp COPYONLY)

add_executable(check_repack check_repack.c greatest.h)
target_include_directories(check_repack PUBLIC include)
target_link_libraries(check_repack PUBLIC libdng check)

add_test(NAME check_mode COMMAND check_mode)
add_test(NAME check_dng COMMAND check_dng)
add_test(NAME check_dcp COMMAND check_dcp)
add_test(NAME check_repack COMMAND check_repack)