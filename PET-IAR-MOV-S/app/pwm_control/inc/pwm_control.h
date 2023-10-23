
#include "pico/stdlib.h"
#define CLOCK_DIV 125.0 ///VALUE  VALID If  1.0<= CLOCK_DIV<256.0
#define TOP_VALUE_COUNT 12500 /// is a uint16_T -> max is a 65535 
typedef struct{
    uint port_l ;
    uint port_r ;
    uint percent_l ; 
    uint percent_r  ; 
}BTS7960_t  ;  

/**
 * @brief 
 * 
 * @param config_ports: define una estructura ....  
 */
void init_pwm(BTS7960_t *config_ports) ; 
void set_pwm (BTS7960_t *config_ports) ; 
void get_pwm (BTS7960_t *config_ports) ; 