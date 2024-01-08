#include "encoder_analog.h" 
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "string.h"
#include "hardware/irq.h"
// internal function 
#define SAMPLES_NUMBER 50 
static volatile uint16_t sample_filter ; 
static uint16_t samples_analog[SAMPLES_NUMBER] ; 
static uint8_t _number_sample = 0 ; 
static int16_t value_zero = 0; 
static int16_t value_max  = 4096; 
static float deltay = 90.0; 
static volatile uint16_t  sample_adc_antenna; 
volatile static uint channel_adc = 0 ; 
volatile static uint16_t reference ; 

encoder_quad_t encoder; 
static int16_t deltax = 4096 ; 
static void order_samples() ; 
static void median_filter() ; 
static volatile void irq_dma_rx(void ) ; 


void getData(encoder_quad_t *quadrature_enc) {
    memcpy(quadrature_enc ,&encoder ,sizeof(encoder_quad_t)) ; 
} 

void set90(){ 
    encoder.angle = MAX_ANGLE;
    encoder.raw_data = sample_filter;   // cuentas equivalentes a 90º 
    encoder.direccion = COUNTER_STILL;
    value_max = encoder.raw_data ; 
    deltay = MAX_ANGLE - MIN_ANGLE ; 
    deltax = value_max - value_zero ; 
}
 




bool init_encoder_analog(uint8_t port_analog_read){
    adc_init() ; 
    //adc_gpio_init(26);
    //adc_select_input(0); 
    adc_set_round_robin(0x03) ; //0b0011   
    adc_fifo_setup(
        true,     // Write each completed conversion to the sample FIFO
        false,    // Enable DMA data request (DREQ)
        1,        // DREQ (and IRQ) asserted when at least 1 sample present
        false,    // We won't see the ERR bit because of 8 bit reads; disable.
        false     // Shift each sample to 8 bits when pushing to FIFO
    );
    adc_set_clkdiv(9600); ///sample t = 0.2 ms
    irq_set_exclusive_handler(ADC_IRQ_FIFO, irq_dma_rx);
    adc_irq_set_enabled(true);
    irq_set_enabled(ADC_IRQ_FIFO, true);
    adc_run(true);
    encoder.state = STATE_00 ; 
    return true ;   
} 



void setZero(){
    encoder.angle = MIN_ANGLE;
    encoder.raw_data = sample_filter;
    value_zero =   encoder.raw_data ; 
    encoder.direccion = COUNTER_STILL; 
}  



uint16_t get_reference(void){ 
    return reference; 
}


static volatile void irq_dma_rx(void ){ 
    channel_adc= adc_get_selected_input() ; 
    if (channel_adc == (uint)0){ 
        reference = (uint16_t)adc_hw->fifo; 
        return ; 
    }else if (channel_adc == (uint)1){
        sample_adc_antenna = (uint16_t)adc_hw->fifo ; 
        samples_analog[_number_sample] = sample_adc_antenna>=reference?sample_adc_antenna:0 ; 
        _number_sample++ ; 
    }else{
        return ; 
    }
    if (_number_sample == SAMPLES_NUMBER){ 
     	//adc_run(false);

        _number_sample = 0 ; 
        median_filter() ; 
     	//adc_run(true);

    }

}




void median_filter(){
    order_samples() ; 
    sample_filter = samples_analog[SAMPLES_NUMBER/2] ; 
    encoder.raw_data = (int16_t)sample_filter ; 
    encoder.angle = (float)((deltay)/deltax)*(float)(encoder.raw_data-value_zero);
}


void order_samples(){ 
    uint16_t i = 0 ; 
    uint16_t min_idx = 0 ; 
    uint16_t j = 0 ; 
    uint16_t temp ; 
    for (i = 0; i < SAMPLES_NUMBER ; i++) { 
        for (j = i + 1; j < SAMPLES_NUMBER; j++)
        { 
            if (samples_analog[i]>samples_analog[j]){ 
                temp = samples_analog[i] ; 
                samples_analog[i] = samples_analog[j] ; 
                samples_analog[j] = temp ; 
            }
        } 

    }
}
