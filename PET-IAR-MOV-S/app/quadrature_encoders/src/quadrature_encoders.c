#include "quadrature_encoders.h"
#include <string.h>
#include "hardware/clocks.h"
#include "hardware/timer.h"


// internal functions ! 
static void gpio_callback_channel_ab(uint pin, uint32_t mask) ; 
static void fsm_encoder(const state_quad_enc_t new_state) ; 
bool alarma(struct repeating_timer *t) ; 
static void read_state() ; 


/// @brief internal variables- > method is oo for C 
static encoder_quad_t encoder ;
static volatile uint8_t count_state  = 0  ; 
static volatile uint16_t _port_channel_a = 0 ; 
static volatile uint16_t _port_channel_b = 0; 
static volatile uint64_t clock_speed ; 
static struct repeating_timer timer0 ; 



bool alarma(struct repeating_timer *t){
    read_state() ; 
    return true  ; 
}


void initPorts(uint port_channel_a, uint port_channel_b){
    ///FIXME: manejar errores de canales a,b asociados a puertos 
    ///       review of registers ! 
    _port_channel_a = port_channel_a     ; 
    _port_channel_b = port_channel_b     ; 
    gpio_init(_port_channel_a)          ;
    gpio_set_dir(_port_channel_a,false) ; 
    gpio_pull_up(_port_channel_a) ; 
    gpio_init(_port_channel_b)  ;
    gpio_set_dir(_port_channel_b,false) ; 
    gpio_pull_up(_port_channel_b) ;     
    encoder.state = ((uint8_t)(gpio_get(_port_channel_a)<<1) |  gpio_get(_port_channel_b)) ; 
    gpio_set_irq_enabled(_port_channel_a,GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE  ,true) ; 
    gpio_set_irq_enabled(_port_channel_b,GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE  ,true) ; 
    gpio_set_irq_callback(&gpio_callback_channel_ab);     

    irq_set_enabled(IO_IRQ_BANK0, true);
    //timer0.alarm_id = 0 ; 
    bool ms_al =  add_repeating_timer_ms((int32_t) 800,&alarma, NULL, &timer0 ) ; 
    ///FIXME: ASSERT IF MS_AL == FALSE ! 
} 
 





void read_state(){ 
    // 0 -> low , !=0 for high --> extract api docs ! 
    uint8_t new_state   =   ((uint8_t)(gpio_get(_port_channel_a)<<1) |  gpio_get(_port_channel_b))   ; 
    encoder.direction = (new_state == encoder.state)?COUNTER_STILL:encoder.direction;  
    count_state = 0 ; 
}



void setZero(){
    encoder.angle = 0;
    encoder.count_pulses = 0;
    //3  ;
    encoder.direction = COUNTER_STILL; 
}  

/**
 * @brief Pone el angulo en 90
 * 
 */
void set90(){
    encoder.angle = 90.0;
    encoder.count_pulses = 90.0/CONVERSION_PULSES_FACTOR;  // Pulsos equivalentes a 90 grados
    encoder.direction = COUNTER_STILL;
}


void fsm_encoder(const state_quad_enc_t new_state){
    switch(encoder.state){
        case STATE_00:           
            if (new_state == STATE_10){
                encoder.state = new_state ; 
                if (encoder.direction == COUNTER_STILL){
                    clock_speed = time_us_64() ; 
                    encoder.direction = COUNTER_CLOCKWISE ; 
                    count_state = 0;
                }else if (encoder.direction ==COUNTER_ANTICLOCKWISE){ 
                    encoder.direction = COUNTER_CLOCKWISE ; 
                    count_state = 0;
                }
                count_state++; 
                if (count_state%4 == 0){
                    clock_speed = time_us_64() - clock_speed ; 
                    encoder.speed = (1/(float) clock_speed) *CONVERSION_PULSES_FACTOR ;
                    count_state = 0; 
                    encoder.count_pulses++ ; 
                    encoder.count_pulses = (encoder.count_pulses + MAX_COUNT_PULSES_PER_REV)%MAX_COUNT_PULSES_PER_REV ; 
                    encoder.angle = encoder.count_pulses*CONVERSION_PULSES_FACTOR ; 
                    clock_speed =  time_us_64()  ; 
                }
            }else if (new_state == STATE_01){
                encoder.state = new_state;
                if (encoder.direction == COUNTER_STILL){
                    clock_speed =  time_us_64()  ; 
                    encoder.direction = COUNTER_ANTICLOCKWISE ; 
                    count_state = 0 ; 
                }else if (encoder.direction == COUNTER_CLOCKWISE){ 
                    encoder.direction = COUNTER_ANTICLOCKWISE ;
                    count_state = 0;        // ESTE RESETEO NO ESTABA - Lo agregue yo (capu) 
                }

                count_state++ ; 
                if (count_state%4 == 0){
                    clock_speed = time_us_64() - clock_speed ; 
                    encoder.speed = -1.0*(1/(float) clock_speed) *CONVERSION_PULSES_FACTOR ;
                    count_state = 0;                     
                    encoder.count_pulses-- ; 
                    encoder.count_pulses = (encoder.count_pulses + MAX_COUNT_PULSES_PER_REV)%MAX_COUNT_PULSES_PER_REV ; 
                    encoder.angle = encoder.count_pulses*CONVERSION_PULSES_FACTOR ; 
                    clock_speed =  time_us_64()  ;  // lo agregue yo
                }
            }
            break ; 
        case STATE_10:
            if (new_state == STATE_11){
                 encoder.state = new_state ; 
                if (encoder.direction == COUNTER_STILL){
                    clock_speed = time_us_64() ; 
                    encoder.direction = COUNTER_CLOCKWISE ; 
                    count_state = 0 ;
                }else if (encoder.direction == COUNTER_ANTICLOCKWISE){ 
                    encoder.direction = COUNTER_CLOCKWISE ; 
                    count_state = 0 ;
                }
                count_state++ ; 
                if (count_state%4 == 0){
                    clock_speed = time_us_64() - clock_speed ; 
                    encoder.speed = (1.0/(float) clock_speed) *CONVERSION_PULSES_FACTOR ;
                    count_state = 0 ; 
                    encoder.count_pulses++ ; 
                    encoder.count_pulses = (encoder.count_pulses + MAX_COUNT_PULSES_PER_REV)%MAX_COUNT_PULSES_PER_REV ; 
                    encoder.angle = encoder.count_pulses*CONVERSION_PULSES_FACTOR ; 
                    clock_speed = time_us_64() ; 
                }
            }else if (new_state == STATE_00){
                encoder.state = new_state ; 

                if (encoder.direction == COUNTER_STILL){
                    encoder.direction = COUNTER_ANTICLOCKWISE ; 
                    clock_speed = time_us_64() ; 
                    count_state = 0 ; 
                }else if (encoder.direction ==COUNTER_CLOCKWISE){ 
                    encoder.direction = COUNTER_ANTICLOCKWISE ; 
                    count_state = 0 ;  
                }
                count_state++ ; 
                if (count_state == 4){
                    clock_speed = time_us_64() - clock_speed ; 
                    encoder.speed = -(1.0/(float) clock_speed) *CONVERSION_PULSES_FACTOR ; 
                    count_state = 0  ; 
                    encoder.count_pulses-- ; 
                    encoder.count_pulses = (encoder.count_pulses + MAX_COUNT_PULSES_PER_REV)%MAX_COUNT_PULSES_PER_REV ; 
                    encoder.angle = encoder.count_pulses*CONVERSION_PULSES_FACTOR ; 
                    clock_speed = time_us_64() ; 
                }

            }

            break ; 
        case STATE_11:
            if (new_state == STATE_01){
                encoder.state = new_state ; 
                if (encoder.direction == COUNTER_STILL){
                    clock_speed = time_us_64() ; 
                    encoder.direction = COUNTER_CLOCKWISE ; 
                    count_state = 0 ;
                }else if (encoder.direction ==COUNTER_ANTICLOCKWISE){ 
                    encoder.direction = COUNTER_CLOCKWISE ; 
                    count_state = 0 ;
                }
                count_state++ ; 
                if (count_state == 4){
                    clock_speed = time_us_64() - clock_speed ; 
                    encoder.speed = (1.0/(float) clock_speed) *CONVERSION_PULSES_FACTOR ; 
                    count_state = 0 ; 
                    encoder.count_pulses++ ; 
                    encoder.count_pulses = (encoder.count_pulses + MAX_COUNT_PULSES_PER_REV)%MAX_COUNT_PULSES_PER_REV ; 
                    encoder.angle = encoder.count_pulses*CONVERSION_PULSES_FACTOR ; 
                    clock_speed = time_us_64() ; 
                }
            }else if (new_state == STATE_10){
                 encoder.state = new_state ; 

                if (encoder.direction == COUNTER_STILL){
                    clock_speed = time_us_64() ; 
                    encoder.direction = COUNTER_ANTICLOCKWISE ; 
                    count_state = 0 ; 
                }else if (encoder.direction ==COUNTER_CLOCKWISE){ 
                    encoder.direction = COUNTER_ANTICLOCKWISE ; 
                    count_state = 0 ;  
                }

                count_state++ ; 
                if (count_state == 4){ 
                    clock_speed = time_us_64() - clock_speed ;
                    encoder.speed = -(1.0/(float) clock_speed) *CONVERSION_PULSES_FACTOR ;  
                    count_state = 0 ; 
                    encoder.count_pulses-- ; 
                    encoder.count_pulses = (encoder.count_pulses + MAX_COUNT_PULSES_PER_REV)%MAX_COUNT_PULSES_PER_REV ; 
                    encoder.angle = encoder.count_pulses*CONVERSION_PULSES_FACTOR ; 
                    clock_speed = time_us_64() ; 
                }
            }
            break ; 
        case STATE_01:
            if (new_state == STATE_00){
                 encoder.state = new_state ; 
                if (encoder.direction == COUNTER_STILL){
                    clock_speed = time_us_64() ; 
                    encoder.direction = COUNTER_CLOCKWISE ; 
                    count_state = 0 ;
                }else if (encoder.direction ==COUNTER_ANTICLOCKWISE){ 
                    encoder.direction = COUNTER_CLOCKWISE ; 
                    count_state = 0 ;
                }
                count_state++ ; 
                if (count_state%4 == 0){
                    clock_speed =time_us_64() - clock_speed ; 
                    encoder.speed = (1.0/(float) clock_speed) *CONVERSION_PULSES_FACTOR ; 
                    count_state = 0 ; 
                    encoder.count_pulses++ ; 
                    encoder.count_pulses = (encoder.count_pulses + MAX_COUNT_PULSES_PER_REV)%MAX_COUNT_PULSES_PER_REV ; 
                    encoder.angle = encoder.count_pulses*CONVERSION_PULSES_FACTOR ; 
                    clock_speed = time_us_64() ; 

                }
            }else if (new_state == STATE_11){
                encoder.state = new_state ; 

                if (encoder.direction == COUNTER_STILL){
                    clock_speed = time_us_64() ; 
                    encoder.direction = COUNTER_ANTICLOCKWISE ; 
                    count_state = 0 ; 
                }else if (encoder.direction ==COUNTER_CLOCKWISE){ 
                    encoder.direction = COUNTER_ANTICLOCKWISE ; 
                    count_state = 0 ;  
                }

                count_state++ ; 
                if (count_state%4 == 0){
                    count_state = 0 ; 
                    clock_speed = time_us_64() - clock_speed ;  
                    encoder.speed = -(1.0/(float) clock_speed) *CONVERSION_PULSES_FACTOR ; 
                    encoder.count_pulses-- ; 
                    encoder.count_pulses = (encoder.count_pulses + MAX_COUNT_PULSES_PER_REV)%MAX_COUNT_PULSES_PER_REV ; 
                    encoder.angle = encoder.count_pulses *CONVERSION_PULSES_FACTOR ; 
                    clock_speed = time_us_64() ;
                }

            }
            break ; 
        default:  //safe code 
            break ; 
    }    
}





static void gpio_callback_channel_ab(uint gpio,uint32_t event_mask ) { 
    uint8_t new_state ; 
  
    if (gpio == _port_channel_a){
        new_state = gpio_get(_port_channel_b) ; 
        if (event_mask == GPIO_IRQ_EDGE_FALL){ 
        ///edge fall -> channel a 1 -> 0
            new_state = 0<<1 | new_state; 
        }else if(event_mask == GPIO_IRQ_EDGE_RISE) {
            ///edge rise -> channel a 0 -> 1 
            new_state = 1<<1 | new_state; 
        }

    }else if (gpio == _port_channel_b){
        new_state =  gpio_get(_port_channel_a)<<1 ; 
        if (event_mask == GPIO_IRQ_EDGE_FALL){ 
            new_state = new_state | 0 ; 

        }else if(event_mask == GPIO_IRQ_EDGE_RISE) {        
            new_state = new_state | 1 ; 
        }


    }else return ; 
    ///FIXME: CHANGE FOR A QUEUE 
    fsm_encoder(new_state); 
}



void getData(encoder_quad_t *quadrature_enc) {
    memcpy(quadrature_enc ,&encoder ,sizeof(encoder_quad_t)) ; 
} 





