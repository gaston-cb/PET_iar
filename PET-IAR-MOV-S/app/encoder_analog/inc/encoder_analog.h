#include <stdint.h>
#include <stdbool.h>
/**
 * MIN_ANGLE = 0.00 
 * MAX_ANGLE = 90.00 -> VERTICAL 
 * MAX_ANGLE = 360.00 -> HORIZONTAL 
 * 
*/
#define MIN_ANGLE 0.00
#define MAX_ANGLE 90.00

typedef enum{
    STATE_00 = 0  , //00
    STATE_01 = 1  , // 1   
    STATE_11 = 3  , // 3
    STATE_10 = 2  , // 2 

}state_quad_enc_t ; 

typedef enum{
    COUNTER_CLOCKWISE,
    COUNTER_ANTICLOCKWISE,
    COUNTER_STILL,  
}direction_t ;  

typedef struct 
{
    int16_t raw_data ; 
    state_quad_enc_t state ; 
    direction_t direccion ; 
    float speed; 
    float angle; 
}encoder_quad_t ;


void setZero(); 
void set90();
void getData(encoder_quad_t *quadrature_enc); 
uint16_t get_reference(void) ; 




/// @brief inicio de puerto de lectura analogico
/// @param port_analog_read 
/// @return : si el puerto es correcto o no
bool init_encoder_analog(uint8_t port_analog_read); 
