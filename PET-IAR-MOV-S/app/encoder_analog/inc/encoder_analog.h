#include <stdint.h>
#include <stdbool.h>

#define ANGLE_MIN 0 /// test using inclinometer 

typedef enum{
    COUNTER_CLOCKWISE,
    COUNTER_ANTICLOCKWISE,
    COUNTER_STILL,  
}direction_t ;  



typedef struct{
    uint16_t zero_value ; 
    uint16_t max_value  ; 
    direction_t sentido ; 
    uint16_t angle_read ; //raw data to adc 
    uint16_t adc_angle_set; 
    float angle         ;    
    float angle_set; 
}encoder_analog_t ; 



/// @brief inicio de puerto de lectura analogico
/// @param port_analog_read 
/// @return : si el puerto es correcto o no
bool init_encoder_analog(uint8_t port_analog_read); 
uint16_t get_sample(encoder_analog_t *encoder) ; 
void set_zero(); 
void set_max_value();
void set_angle(float angle_set) ; 