# cmake -DCMAKE_TOOLCHAIN_FILE=cmake/DevkitArm3DS.cmake ..
set(CMAKE_TOOLCHAIN_FILE cmake/DevkitArm3DS.cmake)
set(project_title DetKit_UI)
cmake_minimum_required(VERSION 3.14)


project(${project_title}.elf)

set(APP_TITLE DetKit_UI)
set(APP_DESCRIPTION "DetKitNCNN with UI backend")
set(APP_AUTHOR Deepdive543443)
set(APP_ICON 48_ncnn.png)
set(ROMFS romfs)

# Note that you must copy the cmake folder and the DevkitArm3DS.cmake file in this directory
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake) # Add the cmake folder to the modules paths, so that we can use the tools and find_package for ctrulib
include(Tools3DS) # Include all the macros and tools needed for 3ds development.
find_package(CTRULIB REQUIRED) # Look for ctrulib

# set(ncnn_DIR ../../../../cpp_libs/ncnn_master_3ds/lib/cmake/ncnn) # Link ncnn
# add_subdirectory(src/detector)

# Create executable
file(GLOB_RECURSE SOURCES src/*)
add_executable(${project_title}.elf ${SOURCES}) # Create the elf file
target_include_directories(${project_title}.elf PRIVATE src)
# file(GLOB SOURCES src/*)
# add_executable(DetKit_DEMO.elf ${SOURCES}) # Create the elf file
# target_include_directories(DetKit_DEMO.elf PRIVATE src)
# target_link_libraries(DetKit_DEMO.elf detkit)


target_link_libraries(${project_title}.elf 3ds::ctrulib) # Link ctrulib, this will also setup the include directories !
# target_include_directories(LVGL-demo.elf PRIVATE ../../cpp_libs/rapidjson-1.1.0/include) # Link RapidJSON


set(ncnn_DIR ../../cpp_libs/ncnn_master_3ds/lib/cmake/ncnn) # Link ncnn
find_package(ncnn REQUIRED)
target_link_libraries(${project_title}.elf ncnn)


add_3dsx_target(${project_title}.elf 
    ${APP_TITLE}
    ${APP_DESCRIPTION}
    ${APP_AUTHOR}
    ${APP_ICON}
    ${ROMFS}
) # Generate the .3dsx and .smdh from the hello_world target