#include "mode_management.h" 
#include "pwm_control.h"
#include "pid_digital.h"
#include "encoder_analog.h"


typedef enum 
{
    ENGINEER, 
    MANUAL, 
    CALIBRATION, 
    AUTOMATIC,
}mode_management_t ;

mode_management_t mode_var  ; 
static BTS7960_t pwm_motor    ; 
static void engineer_mode(const uint8_t *buffer)   ; 
static void manual_mode(const uint8_t *buffer); 
static void calibration_mode(const uint8_t *buffer); 
static void automatic_mode  (const uint8_t *buffer); 
static void motor_ah(const uint16_t pwm_value); 
static void motor_h(const uint16_t pwm_value); 
static void motor_stop(); 






/// @brief  test function and set mode initial 
/// @param mode 
void set_mode(mode_management_t mode){ 
    mode_var = mode ; 
}




void command_receive(const uint8_t *buffer,const uint8_t length_buffer){


    if((char)buffer[0] == 'm')
    {
            set_mode(buffer[1]) ; 
    }

    switch (mode_var)
    {
        case ENGINEER:
            engineer_mode(buffer) ; 
            break;
        case MANUAL:
            manual_mode(buffer) ; 
            break;
        case CALIBRATION:
            calibration_mode(buffer) ; 
            break;
        case AUTOMATIC:
            automatic_mode(buffer) ; 
            break;    
        default:
            break;
    }



}








/*
M -> inicio de búsqueda de min-max pwm valor  
p  -> setear constante proporcional 
i   -> setear constante integral 
d  -> setar  constante derivativa
o  -> corre pid 
l -> setear angulo 
a -> movimiento antihorario 
h -> movimiento horario 
s -> frenar motor (stop motor) 
*/ 
static void engineer_mode(const uint8_t *buffer){
    switch ((char )buffer[0])
    {
        case 'M': /// search min-max pwm 
            break;
        /// constantes proporcional integral y derivativa 
        case 'p':  /// 
            break;
        case 'i':  ///
            break;
        case 'd':  ///
            break ; 
        case 'o':  ///run pid 
            break ;
        case 'u':  ///setting a setpoint value 
            break ; 
        case 'l':  
            break ; 
        case 'a': 
            break ; 
        case 'h': 
            break ; 
        case 's':             
            break ; 
        default:
            break;
    }


}




static void manual_mode(const uint8_t *buffer){
///    a -> movimiento antihorario 
///h -> movimiento horario 
///s -> frenar motor (stop motor) 
    switch ((char)buffer[0])
    {
    case 'a':
        motor_ah(16250) ; 
        break;
    case 'b':
        motor_h(16250) ;         
        break;
    case 's':
        motor_stop() ;         
        break;
    default:
        break;
    }



}

///z -> cero encoder 
///l  -> setear angulo 
///a -> movimiento antihorario 
///h -> movimiento horario 
///s -> frenar motor (stop motor) 
static void calibration_mode(const uint8_t *buffer){
    switch ((char ) buffer[0])
    {
    case 'z':        
        break;
    case 'l':        
        break;
    case 'a':        
        motor_ah(16250) ; 
        break;
    case 'h':        
        motor_h(15250) ; 
        break;
    case 's':
        motor_stop() ;         
        break;
    default:
        break;
    }



}


///P -> park antena 
///	t  -> tracking mode source: 
///una vez enviado esta posición, se debe empezar a enviar el ángulo de setpoint a seguir. 
///u-> set point 
static void automatic_mode(const uint8_t *buffer){
    switch ((char)buffer[0])
    {
    case 'P':
        //park_antenna 
        break;
    case 't': 
        //tracking commands 
        break ; 
    case 'u': 
        //set_point commands 
        break ; 
    default:
        break;
    }
}




/// @brief  use a manual mode and calibration mode 
static void motor_ah(const uint16_t pwm_value){ 
//getPWM -> set the pwm mode 



}

static void motor_h(const uint16_t pwm_value){ 
    
}


static void motor_stop(){ 
    //getpwm() -> l y r en 0 -> set_pwm() ; 
}
