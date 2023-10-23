cmake_minimum_required(VERSION 3.13)



set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include_directories()
add_library(pwm_control
    inc/pwm_control.h 
    src/pwm_control.c)

target_include_directories(pwm_control PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/inc)  


target_link_libraries(pwm_control pico_stdlib hardware_pwm) 
