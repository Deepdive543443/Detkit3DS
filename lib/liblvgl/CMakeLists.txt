cmake_minimum_required(VERSION 3.0.0)
set (CMAKE_CXX_STANDARD 11)
project(lvgl VERSION 0.1.0 LANGUAGES C CXX)

file( GLOB_RECURSE LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/*.c )
add_library( lvgl ${LIB_SOURCES} )
target_include_directories( lvgl PRIVATE ${CMAKE_CURRENT_LIST_DIR}/lvgl)
target_include_directories( lvgl PRIVATE ${CMAKE_CURRENT_LIST_DIR})

install(TARGETS lvgl DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/install)
install(FILES ${CMAKE_CURRENT_LIST_DIR}/lv_conf.h DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/install)
install(FILES ${CMAKE_CURRENT_LIST_DIR}/lvgl/lvgl.h DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/install)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/*.h)
foreach(header ${HEADERS})
    file(RELATIVE_PATH HEADER_REL_PATH ${CMAKE_CURRENT_LIST_DIR}/lvgl/src ${header})
    get_filename_component(barename ${HEADER_REL_PATH} DIRECTORY)
    install(FILES ${header} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/install/src/${barename})
endforeach()

