
#include "pico/stdlib.h" 

#define FC_H 1
#define FC_AH 2

#define LEVEL_DETECTED_CLOSE_SWITCH 1 //LOW -  0 , HIGH =  1  
typedef struct {
    uint16_t limit_switch_port_a ;
    uint16_t limit_switch_port_b ; 
//    uint
}limit_switch_t ; 

//void set_level_switch(uint8_t level_detect) ; 
void init_switch(uint switch_port_a, uint switch_port_b) ; 
uint8_t isSwitchOn() ; 

/**
 * @brief Tiene la función de elegir la forma de detectar el fin de carrera ->low or high level 
 * 
 */
