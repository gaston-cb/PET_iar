cmake_minimum_required(VERSION 3.13)



set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include_directories()
add_library( mode_management
            inc/mode_management.h
            src/mode_management.c)

target_include_directories(mode_management PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/inc)  


target_link_libraries(mode_management encoder_analog limit_switch pid_digital) 
