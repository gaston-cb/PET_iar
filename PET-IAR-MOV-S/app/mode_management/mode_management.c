#include "mode_management.h" 
#include "pwm_control.h"

typedef enum 
{
    ENGINEER, 
    MANUAL, 
    CALIBRATION, 
    AUTOMATIC,
}mode_management_t ;

static mode_managment_t mode_var  ; 
static BTS7960_t pwm_motor    ; 
static void engineer_mode()   ; 
static void manual_mode     (); 
static void calibration_mode(); 
static void automatic_mode  (); 

void set_mode(mode_management_t mode){ 
    mode_var = mode ; 
}



void command_receive(const uint8_t *buffer,const size_t length_buffer){ 

    switch ((char )buffer[0])
    {
        case 'm':

            break;
        default:
            break;
    }        




}









static void engineer_mode(){}


static void manual_mode(){ }
static void calibration_mode(){ }
static void automatic_mode(){}




/// @brief  use a manual mode and calibration mode 
static void motor_ah(){ 
//getPWM -> set the pwm mode 



}

static void motor_h(){ 
//getPWM -> set the pwm mode 
    
}


static void motor_stop(){ 
    //getpwm() -> l y r en 0 -> set_pwm() ; 
}
