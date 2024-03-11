cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

project(myproject_test C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()
#include_directories(../..//PET-IAR-MOV-S/app/ADS1115)
#include_directories(../..//PET-IAR-MOV-S/app/encoder_analog)

add_subdirectory(main)
add_subdirectory(../..//PET-IAR-MOV-S/app/encoder_analog [binary_dir])
#add_subdirectory(../..//PET-IAR-MOV-S/app/ADS1115)



#target_include_directories(encoder_analog INTERFACE 
#     ${CMAKE_CURRENT_LIST_DIR}/../../PET-IAR-MOV-S/app/encoder_analog/
#)
#
# create map/bin/hex file etc.
#pico_add_extra_outputs(myproject_test)