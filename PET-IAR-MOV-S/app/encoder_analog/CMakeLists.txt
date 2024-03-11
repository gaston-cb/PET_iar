cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(..//ADS1115/ [binary_dir])


add_library(encoder_analog
    inc/encoder_analog.h 
    src/encoder_analog.c)

target_include_directories(encoder_analog PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/inc)  

target_link_libraries(encoder_analog
		      ADS1115
                     hardware_gpio           
                     hardware_adc
                     hardware_dma
                     hardware_irq) 
