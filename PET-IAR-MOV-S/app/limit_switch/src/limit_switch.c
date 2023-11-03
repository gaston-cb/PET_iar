#include "limit_switch.h"
#include "hardware/gpio.h"
#include <stdio.h>
static limit_switch_t limit_switch ; 

void init_switch(uint switch_port_a, uint switch_port_b){
    ///FIXME: ASSERT TO A PORT DIGITAL IF HAS ASSIGNED ERROR 
    gpio_init(switch_port_a)          ;
    gpio_set_dir(switch_port_a,false) ; 
    gpio_init(switch_port_b)          ;
    gpio_set_dir(switch_port_b,false) ; 
    gpio_pull_up(switch_port_b) ; 
    gpio_pull_up(switch_port_a) ; 
    
    limit_switch.limit_switch_port_a = switch_port_a ; 
    limit_switch.limit_switch_port_b = switch_port_b ; 
   
}
 
/**
 * @brief Detecta que final de carrera se activa 
 * 
 * @return uint8_t 0: open switch  
 *                 1: close switch port a 
 *                 2: close switch port b 
 */                 
volatile uint8_t isSwitchOn(){
    uint8_t state_return = 0  ; 
    if ( gpio_get( limit_switch.limit_switch_port_a ) != 0){
        printf("-> FIN movimiento horario - FC A\r\n"); 
        state_return = FC_H; 
    } else if (gpio_get( limit_switch.limit_switch_port_b ) != 0){
        printf("-> FIN movimiento antihorario - FC B\r\n");
        state_return = FC_AH; 
    } 
    return state_return ; 
} 