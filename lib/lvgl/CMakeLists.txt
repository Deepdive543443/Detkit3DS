cmake_minimum_required(VERSION 3.0.0)
set (CMAKE_CXX_STANDARD 11)
project(lvgl VERSION 0.1.0 LANGUAGES C CXX)

file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_LIST_DIR}/../lvgl-8.3.11/src/*.h)
foreach(header ${HEADERS})
    file(RELATIVE_PATH HEADER_REL_PATH ${CMAKE_CURRENT_LIST_DIR}/../lvgl-8.3.11/src ${header})
    configure_file(${header} ${CMAKE_CURRENT_BINARY_DIR}/src/${HEADER_REL_PATH} COPYONLY)
endforeach()


configure_file(${CMAKE_CURRENT_LIST_DIR}/lv_conf.h ${CMAKE_CURRENT_BINARY_DIR}/../lv_conf.h COPYONLY)
configure_file(${CMAKE_CURRENT_LIST_DIR}/../lvgl-8.3.11/lvgl.h ${CMAKE_CURRENT_BINARY_DIR}/lvgl.h COPYONLY)

file( GLOB_RECURSE LIB_SOURCES ../lvgl-8.3.11/*.c )
# file( GLOB LIB_HEADERS ../lv_conf.h)
add_library( lvgl ${LIB_SOURCES} )
target_include_directories( lvgl PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/../)

