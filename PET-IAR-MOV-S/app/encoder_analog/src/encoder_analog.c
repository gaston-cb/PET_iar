#include "encoder_analog.h" 
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include <string.h>
// internal function 
#define SAMPLES_NUMBER 17 
#define MIN_ADC_PORT /// 
static volatile uint16_t sample_filter ; 
static encoder_analog_t encoder_analog; 

static uint16_t samples_analog[SAMPLES_NUMBER] ; 
static uint8_t _number_sample = 0 ; 


static void order_samples() ; 
static void median_filter() ; 
static void irq_dma_rx(void ) ; 

static bool isZeroValue  = false ;  
static bool isAngleValue = false ;   




bool init_encoder_analog(uint8_t port_analog_read){
    adc_init() ; 
    adc_gpio_init(port_analog_read);
//adc_select_input(port_analog_read-MIN_ADC_PORT); 
    ///FIXME: change with adc automatic selection using number 
    adc_select_input(2); 
    adc_fifo_setup(
        true,    // Write each completed conversion to the sample FIFO
        false,    // Enable DMA data request (DREQ)
        1,       // DREQ (and IRQ) asserted when at least 1 sample present
        false,    // We won't see the ERR bit because of 8 bit reads; disable.
        false     // Shift each sample to 8 bits when pushing to FIFO
    );
    adc_set_clkdiv(9600);
    irq_set_exclusive_handler(ADC_IRQ_FIFO, irq_dma_rx);
	adc_irq_set_enabled(true);
	irq_set_enabled(ADC_IRQ_FIFO, true);
	adc_run(true);
    return true ;   

} 

uint16_t get_sample(encoder_analog_t *encoder){ 
    if (isAngleValue==true && isZeroValue==true){ 
        ///COMPUTE ANGLE 
    }
    memcpy(encoder,&encoder_analog,sizeof(encoder_analog_t)) ; 

}

void set_zero(){ 
    isZeroValue = true ; 
    encoder_analog.zero_value = samples_analog[SAMPLES_NUMBER/2] ; 
}


void set_max_value(){ 
    encoder_analog.max_value = samples_analog[SAMPLES_NUMBER/2] ; 

}


void irq_dma_rx(void ){ 
    samples_analog[_number_sample] =  (uint16_t)adc_hw->fifo;
    _number_sample++ ; 
    if (_number_sample == SAMPLES_NUMBER){ 
        _number_sample = 0 ; 
        median_filter() ; 
    }

}


void set_angle(float angle_set){ 
    adc_run(false) ; 
    isAngleValue = true ; 
    encoder_analog.adc_angle_set = samples_analog[SAMPLES_NUMBER/2] ; 
    encoder_analog.angle_set = angle_set ; 
    adc_run(true) ; 



}

void median_filter(){
    order_samples() ; 
    encoder_analog.angle_read = samples_analog[SAMPLES_NUMBER/2] ; 
    //encoder_analog.angle = (90.0/encoder_analog.max_value-encoder_analog.zero_value)*(encoder_analog.angle_read -encoder_analog.zero_value)    ; 
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
