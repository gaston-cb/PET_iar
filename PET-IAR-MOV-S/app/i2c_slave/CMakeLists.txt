cmake_minimum_required(VERSION 3.13)



set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include_directories()
add_library(i2c_slave
    inc/i2c_slave.h 
    src/i2c_slave.c)

target_include_directories(i2c_slave PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/inc)  


target_link_libraries(i2c_slave 
                      pico_stdlib
                      hardware_dma
                      hardware_i2c)  
