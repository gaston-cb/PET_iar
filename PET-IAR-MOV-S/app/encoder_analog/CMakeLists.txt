cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include_directories()
add_library(encoder_analog
    inc/encoder_analog.h 
    src/encoder_analog.c)

target_include_directories(encoder_analog PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/inc)  

target_link_libraries(encoder_analog
                     hardware_gpio           
                     hardware_adc
                     hardware_dma
                     hardware_irq) 