cmake_minimum_required(VERSION 3.0.0)
set (CMAKE_CXX_STANDARD 11)
project(lvgl VERSION 0.1.0 LANGUAGES C CXX)


set(APP_ICON ../rom/icon_detkit.png)
set(ROMFS ../romfs)
set(banner ../rom/banner_detkit.png)
set(audio ../rom/sonar.wav)
set(RSF test.rsf)

file(GLOB TEST *.c)
foreach(case ${TEST})
    get_filename_component(test_we ${case} NAME_WE)

    set(APP_TITLE ${test_we})
    set(APP_DESCRIPTION "testing ${test_we}...")
    
    add_executable(${test_we}.elf ${case} ${SECTIONS} ${ASSET})
    target_include_directories(${test_we}.elf PRIVATE ../src)
    target_include_directories(${test_we}.elf PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/../lib)
    
    target_link_libraries(${test_we}.elf 3ds::citro2d)
    target_link_libraries(${test_we}.elf 3ds::citro3d)
    target_link_libraries(${test_we}.elf 3ds::ctrulib)
    target_link_libraries(${test_we}.elf m)
    
    #Link detkit
    target_include_directories(${test_we}.elf PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../lib/build/detector)
    target_link_libraries(${test_we}.elf ${CMAKE_CURRENT_LIST_DIR}/../lib/build/detector/libdetkit.a)
    
    #Link LVGL
    target_include_directories(${test_we}.elf PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../lib/build/lvgl)
    target_link_libraries(${test_we}.elf ${CMAKE_CURRENT_LIST_DIR}/../lib/build/lvgl/liblvgl.a)
    
    set(ncnn_DIR ${CMAKE_CURRENT_LIST_DIR}/../lib/build/ncnn_3ds_master/lib/cmake/ncnn) # Link ncnn(temp)
    find_package(ncnn REQUIRED)
    target_link_libraries(${test_we}.elf ncnn)

    add_3dsx_target(${test_we}.elf
        ${APP_TITLE}
        ${APP_DESCRIPTION}
        ${APP_AUTHOR}
        ${APP_ICON}
        ${ROMFS}
    )

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${test_we}.3dsx DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../3dsx)
    if(BUILD_CIA)
        add_cia_target(${test_we}.elf ${RSF} ${banner} ${audio})
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${test_we}.cia DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../cia)
    endif()
endforeach()
