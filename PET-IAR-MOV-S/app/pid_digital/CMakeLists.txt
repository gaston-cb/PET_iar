cmake_minimum_required(VERSION 3.13)



set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include_directories()
add_library(pid_digital
    inc/pid_digital.h 
    src/pid_digital.c)

target_include_directories(pid_digital PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/inc)  


target_link_libraries(pid_digital 
                      pico_stdlib 
                      pwm_control 
                      encoder_analog 
                      pico_stdlib
                      limit_switch) 
