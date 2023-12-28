cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

project(myproject_test C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

#include_directories(${CMAKE_CURRENT_LIST_DIR}/../../PET-IAR-MOV-S/app/encoder_analog/)

# add_library(
#     encoder_analog
#     ${CMAKE_CURRENT_LIST_DIR}/../../PET-IAR-MOV-S/app/encoder_analog/inc/encoder_analog.h
#     ${CMAKE_CURRENT_LIST_DIR}/../../PET-IAR-MOV-S/app/encoder_analog/src/encoder_analog.c
# )

#target_include_directories(encoder_analog PUBLIC 
#     ${CMAKE_CURRENT_LIST_DIR}/../../PET-IAR-MOV-S/app/encoder_analog/inc/
#)
# target_link_libraries(encoder_analog
#                      hardware_gpio           
#                      hardware_adc
#                      hardware_dma
#                      hardware_irq) 





add_subdirectory(main)

add_subdirectory(../..//PET-IAR-MOV-S/app/encoder_analog [binary_dir])



#target_include_directories(encoder_analog INTERFACE 
#     ${CMAKE_CURRENT_LIST_DIR}/../../PET-IAR-MOV-S/app/encoder_analog/
#)
#
# create map/bin/hex file etc.
#pico_add_extra_outputs(myproject_test)