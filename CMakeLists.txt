# Build all objects using cmake -DBUILD_TEST=ON -DBUILD_CIA=ON ..
set(CMAKE_TOOLCHAIN_FILE cmake/DevkitArm3DS-toolchain.cmake)
set(project_title DetKit3DS)
cmake_minimum_required(VERSION 3.14)

set(APP_TITLE ${project_title})

# Rom components(icon, sound effect, rom file system, etc)
set(APP_DESCRIPTION "DetKitNCNN with UI backend")
set(APP_AUTHOR Deepdive543443)
set(APP_ICON rom/icon_detkit.png)
set(RSF rom/homebrew.rsf)
set(banner rom/banner_detkit.png)
set(audio rom/sonar.wav)
set(ROMFS romfs)

option(BUILD_CIA "Build CIA package for title manager to install" OFF)
option(BUILD_TEST "Build test case of different experimental features" OFF)
option(USE_SYS_CORE "Made threads on sys core" OFF)

if(BUILD_CIA)
    add_definitions(-DBUILD_CIA)
endif()

if(USE_SYS_CORE)
    add_definitions(-DUSE_SYS_CORE)
endif()



project(${project_title}.elf)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
include(Tools3DS) # Include all the macros and tools needed for 3ds development.
find_package(CTRULIB REQUIRED)
find_package(CITRO3D REQUIRED)
find_package(CITRO2D REQUIRED)

# Build executable
file(GLOB_RECURSE SECTIONS src/sections/*.c)
add_executable(${project_title}.elf src/main.c ${SECTIONS})
target_include_directories(${project_title}.elf PRIVATE src)

# Link ctrulib, this will also setup the include directories
target_link_libraries(${project_title}.elf 3ds::citro2d)
target_link_libraries(${project_title}.elf 3ds::citro3d)
target_link_libraries(${project_title}.elf 3ds::ctrulib)
target_link_libraries(${project_title}.elf m)

#Link detkit
target_include_directories(${project_title}.elf PRIVATE ${CMAKE_CURRENT_LIST_DIR}/lib/build/detector)
target_link_libraries(${project_title}.elf ${CMAKE_CURRENT_LIST_DIR}/lib/build/detector/libdetkit_c.a)

#Link LVGL
target_include_directories(${project_title}.elf PRIVATE ${CMAKE_CURRENT_LIST_DIR}/lib/build/lvgl)
target_link_libraries(${project_title}.elf ${CMAKE_CURRENT_LIST_DIR}/lib/build/lvgl/liblvgl.a)

# Link ncnn
set(ncnn_DIR ${CMAKE_CURRENT_LIST_DIR}/lib/build/ncnn_3ds/lib/cmake/ncnn)
find_package(ncnn REQUIRED)
target_link_libraries(${project_title}.elf ncnn)

add_3dsx_target(${project_title}.elf 
    ${APP_TITLE}
    ${APP_DESCRIPTION}
    ${APP_AUTHOR}
    ${APP_ICON}
    ${ROMFS}
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${project_title}.3dsx DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/3dsx)

if(BUILD_CIA)
    add_cia_target(${project_title}.elf ${RSF} ${banner} ${audio})
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${project_title}.cia DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/cia)
endif()

if(BUILD_TEST)
    add_subdirectory(test)
endif()