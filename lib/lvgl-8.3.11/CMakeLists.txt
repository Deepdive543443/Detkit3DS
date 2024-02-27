cmake_minimum_required(VERSION 3.0.0)
set (CMAKE_CXX_STANDARD 11)
project(lvgl VERSION 0.1.0 LANGUAGES C CXX)

configure_file(${CMAKE_CURRENT_LIST_DIR}/lv_conf.h ${CMAKE_CURRENT_BINARY_DIR}/../lv_conf.h COPYONLY)

file( GLOB_RECURSE LIB_SOURCES ../lvgl/*.c )
file( GLOB LIB_HEADERS ../lv_conf.h)
add_library( lvgl ${LIB_SOURCES} ${LIB_HEADERS} )
target_include_directories( lvgl PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/../)

