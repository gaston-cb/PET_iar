cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

project(myproject_test C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()


add_subdirectory(main)
add_subdirectory(app/encoder_analog) 
add_subdirectory(app/pwm_control)  
add_subdirectory(app/pid_digital)   
add_subdirectory(app/i2c_slave) 
add_subdirectory(app/limit_switch)   
add_subdirectory(app/mode_management)   

# create map/bin/hex file etc.
#pico_add_extra_outputs(myproject_test)