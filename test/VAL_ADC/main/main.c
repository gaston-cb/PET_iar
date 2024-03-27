/**
 * @file main.c
 * @authors gaston valdez - Dario Capucchio
 * @brief
 * @version 0.1
 * @date 2023-03-17
 * @copyright
 */
/*================ LIBRERIAS =================================================================*/
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "encoder_analog.h"
/*================ DEFINICIONES ==============================================================*/
encoder_quad_t enc_test;
uint16_t raw_data_analog[2000] ;
uint16_t reference[2000]; 
uint16_t difference[2000] ;  
float angle[2000] ; 
void read_angle(); 
void sample_reference() ; 
void difference_adc() ; 
void processing_and_send_samples() ; 
/*================ MAIN CORE 0 ===============================================================*/
/**
 * ADC = (vIN/vREF)*2^N 
 * vIN = adc/2^n *Vref 
 * 
*/
int16_t error_fn(int16_t data){
    int16_t fec = 0 ; 
    if (data<=256){
        fec = 10 ; 
    }else if(data>256 && data<=501 ){ 
        fec= 11 ; 
    }else if(data>501 && data<=570){
        fec = (float) 0.15942*(data-570) ; // eq    
    }else if (data>1471 && data<=1708){
        fec = -8 ; 
    }else if(data>1708 && data<1952){
        fec = (data>1829)?-10:-9 ; 
    }else if(data>1952 && data<=2444){
        fec = -7 ; 
    }else if(data>2444 && data<2559){
        fec =  (float)-0.06956*(data-2444)-8 ; 
    }else if(data>=2556 && data<2684){
        fec = -16 ;  
    }else if(data>2684 && data<3544){
        fec = -13 ;  
    }else if(data>=3544 && data<3657){
        fec = (float)-0.09734*(data-3544)-12 ; 
    }else if (data>=3657 && data<4025){
        fec = -23 ; 
    }
    return fec; 




}



void main()

{
    stdio_init_all();
    // gpio_init(23) ; 
    // gpio_set_dir(23, GPIO_OUT);
    // gpio_put(23, 1); ///PWM mode 
    init_encoder_analog(28); 
    char c ;
    int16_t error ;
    float voltage ; 
    float voltage_corr ; 

    while (1)
    {
        getData(&enc_test) ;
        voltage = (3.3/4096)*  (float)enc_test.raw_data ;   
        voltage_corr = (3.3/4096)*  (float)(enc_test.raw_data+error) ;   
        printf("raw_data: %d - raw_data_fec: %d - error = %d, volts = %.4f, volt_corr = %.4f, angleH=%.4f\r\n",
                enc_test.raw_data,
                enc_test.raw_data - error_fn(enc_test.raw_data),
                error_fn( enc_test.raw_data), 
                voltage,
                voltage_corr, 
                enc_test.angle
                ) ; 
        sleep_ms(1000) ; 
//        c = getchar() ; 
        // switch (c)
        // {
        //     case 'a': 
        //         printf("leyendo lecturas de adc dato crudo\r\n"); 
        //         processing_and_send_samples() ; 
        //         break;
        //     case 'r': 
        //         printf("leyendo lecturas de adc dato crudo\r\n"); 
        //         sample_reference() ; 
        //         break;
        //     case 'd':
        //         difference_adc() ; 
        //         break ; 
        //     case 'Z':       
        //         setZero() ; 
        //         break ; 
        //     case 'z':       
        //         set90();     
        //         break;
        //     case 'b':
        //         printf("reading angle\r\n") ; 
        //         read_angle() ; 
        //         break ; 
        //     default:
        //         break;
        // }
     }

}


void processing_and_send_samples(){ 
    int i = 0 ;     
    for (i =0 ; i<2000;i++){ 
        sleep_ms(200) ; 
        getData(&enc_test) ; 
        raw_data_analog[i] = enc_test.raw_data ; 
    }

    for (i =0 ; i<2000;i++){ 
        printf("%d,%04x,%d\r\n",i,raw_data_analog[i],raw_data_analog[i]) ; 
    }    

    printf("final de archivo"); 

}


void sample_reference(){
    int i = 0 ; 
    for (i =0 ; i<2000;i++){ 
        sleep_ms(200) ; 
        reference[i] = get_reference() ; 
    }

    for (i =0 ; i<2000;i++){ 
        printf("%d,%04x,%d\r\n",i,reference[i],reference[i]) ; 
        sleep_ms(1) ; 
    }
} 



void difference_adc() { 
    int i = 0 ; 
    memset(raw_data_analog,0,2000) ; 
    memset(reference,0,2000) ; 
    for (i = 0 ; i<2000;i++){ 
        sleep_ms(200) ; 
        getData(&enc_test) ; 
        raw_data_analog[i] = enc_test.raw_data ; 
        reference[i] = get_reference() ; 
        difference[i] = raw_data_analog[i] - reference[i] ; 
    }

    for (i =0 ; i<2000;i++){ 
        printf("%d,%04x,%d,%04x,%d,%04x,%d\r\n",i,
            difference[i],difference[i],
            reference[i], reference[i], 
            raw_data_analog[i],raw_data_analog[i] 
        ) ; 
        sleep_ms(1) ; 
    }

}


void read_angle(){ 
    int i = 0 ; 
    
    for (i = 0 ; i<2000;i++){ 
        sleep_ms(200) ; 
        getData(&enc_test) ; 
        angle[i] = enc_test.angle; 
        raw_data_analog[i] = enc_test.raw_data ; 

    }
    for (i =0 ; i<2000;i++){ 
        printf("%d,%f,%d\r\n",i,angle[i],raw_data_analog[i]) ; 
        sleep_ms(1) ;
    }
}