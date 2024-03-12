#include "encoder_analog.h" 
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "string.h"
#include "hardware/irq.h"
#include "ADS1115.h"
#define PORT_READ_ADC 27


// internal function 
#define SAMPLES_NUMBER 50 
//// constants of calibrations 
// limits 
#define LIMIT1 642
#define LIMIT2 1276
#define LIMIT3 1901
#define LIMIT4 2538
#define LIMIT5 3166
/// constants 
#define CONSTANT_OF_CAL_ADC1 0.9662907946458431
#define CONSTANT_OF_CAL_ADC2 0.9724502193805911
#define CONSTANT_OF_CAL_ADC3 0.9789462270047719
#define CONSTANT_OF_CAL_ADC4 0.9779803871715954
#define CONSTANT_OF_CAL_ADC5 0.9800293889184183
#define CONSTANT_OF_CAL_ADC6 1.068084035553315






static volatile uint16_t sample_filter ; 
static uint16_t samples_analog[SAMPLES_NUMBER] ; 
static uint8_t _number_sample = 0 ; 
static int16_t value_zero = 0; 
static int16_t value_max  = 26400; 
static float deltay = MAX_ANGLE; 
static volatile uint16_t  sample_adc_antenna; 
volatile static uint channel_adc = 0 ; 
volatile static uint16_t reference ; 
encoder_quad_t encoder; 
static int16_t deltax = 4096 ; //change N bits ADC for 3.3V. Depends of AGC of adc 
static uint8_t read_adc_raw[2] ;



static void gpio_callback_channel_ab(uint gpio,uint32_t event_mask ) ; 
//static void order_samples() ; 
//static void median_filter() ; 
//static volatile void irq_dma_rx(void ) ; 








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
    // adc_init() ; 
    //adc_gpio_init(26);
    //adc_select_input(0); 
    // adc_set_round_robin(0x03) ; //0b0011   
    // adc_fifo_setup(
    //     true,     // Write each completed conversion to the sample FIFO
    //     false,    // Enable DMA data request (DREQ)
    //     1,        // DREQ (and IRQ) asserted when at least 1 sample present
    //     false,    // We won't see the ERR bit because of 8 bit reads; disable.
    //     false     // Shift each sample to 8 bits when pushing to FIFO
    // );
    // adc_set_clkdiv(9600); ///sample t = 0.2 ms
    // irq_set_exclusive_handler(ADC_IRQ_FIFO, irq_dma_rx);
    // adc_irq_set_enabled(true);
    // irq_set_enabled(ADC_IRQ_FIFO, true);
    // adc_run(true);
    gpio_init(PORT_READ_ADC)          ;
    gpio_set_dir(PORT_READ_ADC,false) ; 
    gpio_pull_up(PORT_READ_ADC) ; 
    
    ADS1115_alert_comparator_t alert = {
        CONVERSION_READY, 
        1, 
        0x8000, 
        0x0000
    } ; 

    ADS1x15_config_t adc_cfg = {
                CHANNEL_0_GND,
                FSR_4096  ,
                CONTINIOUS_MODE ,
                SPS_860,
                alert
    };   
    


    gpio_set_irq_enabled(PORT_READ_ADC,GPIO_IRQ_EDGE_RISE  ,true) ; 
    gpio_set_irq_callback(&gpio_callback_channel_ab);     
    irq_set_enabled(IO_IRQ_BANK0, true);
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

static void gpio_callback_channel_ab(uint gpio,uint32_t event_mask ) { 
    getVoltage(read_adc_raw) ; //2 bytes 
    sample_filter =   (uint16_t) read_adc_raw[1] <<8   |(uint16_t) read_adc_raw[0] ; 
    encoder.raw_data = (int16_t) sample_filter ;  
    encoder.angle = (float)((deltay)/deltax)*(float)(encoder.raw_data-value_zero);
    if (encoder.angle<=MIN_ANGLE){
        encoder.angle = 0.00 ; 
    }else if(encoder.angle>=MAX_ANGLE){
        encoder.angle = MAX_ANGLE ; 
    }

}






//static volatile void irq_dma_rx(void ){ 
    // channel_adc= adc_get_selected_input() ; 
    // if (channel_adc == (uint)0){ 
    //     reference = (uint16_t)adc_hw->fifo; 
    //     return ; 
    // }else if (channel_adc == (uint)1){
    //     sample_adc_antenna = (uint16_t)adc_hw->fifo ; 
    //     samples_analog[_number_sample] = sample_adc_antenna>=reference?sample_adc_antenna:0 ; 
    //     _number_sample++ ; 

    // }else{
    //     return ; 
    // }
    // if (_number_sample == SAMPLES_NUMBER){ 
    //  	//adc_run(false);
    //     //promedio() ; 
    //     _number_sample = 0 ; 
    //     median_filter() ; 
    //  	//adc_run(true);

    // }
//}




void median_filter()
{
    //order_samples() ; 
    //sample_filter = samples_analog[SAMPLES_NUMBER/2] ; 
    // if (sample_filter<LIMIT1){
    //     sample_filter = sample_filter*CONSTANT_OF_CAL_ADC1 ; 
    // }else if(sample_filter>=LIMIT1 && sample_filter<LIMIT2){
    //     sample_filter = sample_filter*CONSTANT_OF_CAL_ADC2 ; 
    // }else if(sample_filter>=LIMIT2 && sample_filter<LIMIT3){
    //     sample_filter = sample_filter*CONSTANT_OF_CAL_ADC3 ; 
    // }else if(sample_filter>=LIMIT3 && sample_filter<LIMIT4){
    //     sample_filter = sample_filter*CONSTANT_OF_CAL_ADC4 ; 
    // }else if(sample_filter>=LIMIT4 && sample_filter<=LIMIT5){
    //     sample_filter = sample_filter*CONSTANT_OF_CAL_ADC5 ; 
    // }else{
    //     sample_filter = CONSTANT_OF_CAL_ADC6*  ((float)sample_filter- (float)4095)+4095 ;
    // }

    
    encoder.raw_data = (int16_t) sample_filter ;  
    encoder.angle = (float)((deltay)/deltax)*(float)(encoder.raw_data-value_zero);
    if (encoder.angle<=MIN_ANGLE){
        encoder.angle = 0.00 ; 
    }else if(encoder.angle>=MAX_ANGLE){
        encoder.angle = MAX_ANGLE ; 
    }
}


void order_samples(){ 
    // uint16_t i = 0 ; 
    // uint16_t min_idx = 0 ; 
    // uint16_t j = 0 ; 
    // uint16_t temp ; 
    // for (i = 0; i < SAMPLES_NUMBER ; i++) { 
    //     for (j = i + 1; j < SAMPLES_NUMBER; j++)
    //     { 
    //         if (samples_analog[i]>samples_analog[j]){ 
    //             temp = samples_analog[i] ; 
    //             samples_analog[i] = samples_analog[j] ; 
    //             samples_analog[j] = temp ; 
    //         }
    //     } 

    // }
}