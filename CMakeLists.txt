cmake_minimum_required(VERSION 3.25)
project(libdng C)

set(CMAKE_C_STANDARD 23)
set(LIBRARY_VERSION_MAJOR 0)
set(LIBRARY_VERSION_STRING 0.1)
set(CMAKE_C_VISIBILITY_PRESET hidden)

add_library(libdng SHARED include/libdng.h src/libdng.c src/dng.h)

set_target_properties(libdng PROPERTIES
        VERSION ${LIBRARY_VERSION_STRING}
        SOVERSION ${LIBRARY_VERSION_MAJOR}
        PUBLIC_HEADER include/libdng.h)
target_include_directories(libdng PUBLIC include)
target_link_libraries(libdng "tiff")

add_executable(makedng util/makedng.c)
target_include_directories(makedng PUBLIC include)
target_link_libraries(makedng PUBLIC libdng)
