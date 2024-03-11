cmake_minimum_required(VERSION 3.13)



set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)


add_library(portsRP2040 
    src/portsRP2040.c
    inc/portsRP2040.h)

target_include_directories(portsRP2040 PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/inc)  

target_link_libraries(portsRP2040 
    pico_stdlib
    hardware_i2c)  


# end ports FIle 


add_library(
    ADS1115
    inc/ADS1115.h
    src/ADS1115.c
)


target_include_directories(ADS1115 PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/inc)  


target_link_libraries(

    ADS1115
    pico_stdlib
    portsRP2040
)