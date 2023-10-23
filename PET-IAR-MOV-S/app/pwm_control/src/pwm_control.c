#include "string.h"
#include "hardware/pwm.h"
#include "pwm_control.h"


static BTS7960_t pwm_ports  ;  
static  uint slice_num_0 ;  
static  uint slice_num_1 ;  
static  bool isInit = false ; 

void init_pwm(BTS7960_t *config_port ) 
{
///FIXME: AGREGAR MANEJEO DE ERRORES - > ASERT O PUNTERO A FUNCIONES 
    memcpy(&pwm_ports ,config_port,sizeof(BTS7960_t))    ; 
    gpio_set_function(pwm_ports.port_r, GPIO_FUNC_PWM)   ;
    gpio_set_function(pwm_ports.port_l, GPIO_FUNC_PWM)   ;   
    slice_num_0 = pwm_gpio_to_slice_num(pwm_ports.port_r);
    slice_num_1 = pwm_gpio_to_slice_num(pwm_ports.port_l);
    pwm_set_chan_level(slice_num_0, PWM_CHAN_A, 0)       ;
    pwm_set_chan_level(slice_num_1, PWM_CHAN_A, 0)       ;
    pwm_set_wrap(slice_num_0, TOP_VALUE_COUNT)           ;
    pwm_set_wrap(slice_num_1, TOP_VALUE_COUNT)           ;
    pwm_set_enabled(slice_num_0,true)                    ; 
    pwm_set_enabled(slice_num_1,true)                    ; 
    pwm_set_gpio_level (pwm_ports.port_r, pwm_ports.percent_r) ; 
    pwm_set_gpio_level (pwm_ports.port_l, pwm_ports.percent_l) ; 
    isInit = true ; 
}

//n*T = 0.1ms -> n = 0.1ms/T ->
void set_pwm(BTS7960_t *config_port ){ 
    ///assert(isInit!=true) ; 
    memcpy(&pwm_ports ,config_port,sizeof(BTS7960_t))          ; 
    pwm_set_gpio_level (pwm_ports.port_r, pwm_ports.percent_r) ; 
    pwm_set_gpio_level (pwm_ports.port_l, pwm_ports.percent_l) ; 
}






void get_pwm(BTS7960_t *config_port){
    
    memcpy(config_port,&pwm_ports ,sizeof(BTS7960_t)) ; 
}

