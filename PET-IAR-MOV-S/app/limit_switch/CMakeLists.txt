cmake_minimum_required(VERSION 3.13)



set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include_directories()
add_library(limit_switch
    inc/limit_switch.h 
    src/limit_switch.c)

target_include_directories(limit_switch PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/inc)  


target_link_libraries(limit_switch pico_stdlib hardware_gpio) 
