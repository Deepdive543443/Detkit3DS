# Build all objects using cmake -DBUILD_TEST=ON -DBUILD_CIA=ON ..
set(CMAKE_TOOLCHAIN_FILE cmake/DevkitArm3DS.cmake)
set(project_title DetKit3DS)
cmake_minimum_required(VERSION 3.14)

set(APP_TITLE ${project_title})
set(APP_DESCRIPTION "DetKitNCNN with UI backend")
set(APP_AUTHOR Deepdive543443)
set(APP_ICON rom/icon_detkit.png)
set(RSF rom/homebrew.rsf)
set(banner rom/banner_detkit.png)
set(audio rom/sonar.wav)
set(ROMFS romfs)

option(BUILD_CIA "Build CIA package for title manager to install" OFF)
option(BUILD_TEST "Build test case of different experimental features" OFF)

project(${project_title}.elf)
# Note that you must copy the cmake folder and the DevkitArm3DS.cmake file in this directory
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake) # Add the cmake folder to the modules paths, so that we can use the tools and find_package for ctrulib
include(Tools3DS) # Include all the macros and tools needed for 3ds development.
find_package(CTRULIB REQUIRED) # Look for ctrulib

# Build executable
file(GLOB SECTIONS src/sections/*.c)
add_executable(${project_title}.elf src/main.c ${SECTIONS}) # Create the elf file
target_include_directories(${project_title}.elf PRIVATE src)
# target_include_directories(${project_title}.elf PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/lib)

target_link_libraries(${project_title}.elf 3ds::ctrulib) # Link ctrulib, this will also setup the include directories !

#Link detkit
target_include_directories(${project_title}.elf PRIVATE ${CMAKE_CURRENT_LIST_DIR}/lib/build/detector)
target_link_libraries(${project_title}.elf ${CMAKE_CURRENT_LIST_DIR}/lib/build/detector/libdetkit.a)

#Link LVGL
target_include_directories(${project_title}.elf PRIVATE ${CMAKE_CURRENT_LIST_DIR}/lib/build/lvgl)
target_link_libraries(${project_title}.elf ${CMAKE_CURRENT_LIST_DIR}/lib/build/lvgl/liblvgl.a)

set(ncnn_DIR ${CMAKE_CURRENT_LIST_DIR}/lib/ncnn_3ds_master/lib/cmake/ncnn) # Link ncnn(temp)
find_package(ncnn REQUIRED)
target_link_libraries(${project_title}.elf ncnn)

add_3dsx_target(${project_title}.elf 
    ${APP_TITLE}
    ${APP_DESCRIPTION}
    ${APP_AUTHOR}
    ${APP_ICON}
    ${ROMFS}
)

if(BUILD_CIA)
    add_cia_target(${project_title}.elf ${RSF} ${banner} ${audio})
endif()

if(BUILD_TEST)
    add_subdirectory(test)
endif()