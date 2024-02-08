cmake_minimum_required(VERSION 3.0.0)
set (CMAKE_CXX_STANDARD 11)
project(lvgl VERSION 0.1.0 LANGUAGES C CXX)


file( GLOB_RECURSE LIB_SOURCES ../lvgl/*.c )
file( GLOB LIB_HEADERS ../*.h)
add_library( lvgl ${LIB_SOURCES} ${LIB_HEADERS} )
