cmake_minimum_required(VERSION 3.0.0)
set (CMAKE_CXX_STANDARD 11)
project(lvgl VERSION 0.1.0 LANGUAGES C CXX)


set(APP_ICON ../rom/icon_detkit.png)
set(ROMFS ../romfs)

file(GLOB TEST *.c)
foreach(case ${TEST})
    get_filename_component(test_we ${case} NAME_WE)
    add_executable(${test_we}.elf ${case} ${SECTIONS} ${ASSET})
    target_include_directories(${test_we}.elf PRIVATE ../../../cpp_libs/stb-master)
    target_include_directories(${test_we}.elf PRIVATE ../src)

    target_link_libraries(${test_we}.elf 3ds::ctrulib) # Link ctrulib, this will also setup the include directories !
    target_link_libraries(${test_we}.elf detkit)
    target_link_libraries(${test_we}.elf lvgl)

    add_3dsx_target(${test_we}.elf)
endforeach()
