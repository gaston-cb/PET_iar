cmake_minimum_required(VERSION 3.13)



set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include_directories()
add_library(quadrature_encoders
    inc/quadrature_encoders.h 
    src/quadrature_encoders.c)

target_include_directories(quadrature_encoders PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/inc)  


target_link_libraries(quadrature_encoders pico_stdlib hardware_clocks hardware_timer hardware_irq) 

