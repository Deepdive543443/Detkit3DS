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

project(${project_title}.elf)
# Note that you must copy the cmake folder and the DevkitArm3DS.cmake file in this directory
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake) # Add the cmake folder to the modules paths, so that we can use the tools and find_package for ctrulib
include(Tools3DS) # Include all the macros and tools needed for 3ds development.
find_package(CTRULIB REQUIRED) # Look for ctrulib

# Build sub modules
set(ncnn_DIR ../../../../cpp_libs/ncnn_master_3ds/lib/cmake/ncnn) # Link ncnn
add_subdirectory(src/detector)
add_subdirectory(src/lvgl-8.3.11)

# Create executable
file(GLOB SECTIONS src/sections/*.c)
file(GLOB ASSET src/asset/*)
add_executable(${project_title}.elf src/main.c ${SECTIONS} ${ASSET}) # Create the elf file
target_include_directories(${project_title}.elf PRIVATE src)

target_link_libraries(${project_title}.elf 3ds::ctrulib) # Link ctrulib, this will also setup the include directories !
target_link_libraries(${project_title}.elf detkit)
target_link_libraries(${project_title}.elf lvgl)

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