cmake_minimum_required(VERSION 3.0.0)
set (CMAKE_CXX_STANDARD 11)
project(detkit VERSION 0.1.0 LANGUAGES C CXX)


file( GLOB LIB_SOURCES *.c )
file( GLOB LIB_HEADERS *.h )
add_library( detkit ${LIB_SOURCES} ${LIB_HEADERS} )

find_package(ncnn REQUIRED)
target_link_libraries(detkit ncnn)
