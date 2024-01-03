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
void processing_and_send_samples() ; 
/*================ MAIN CORE 0 ===============================================================*/
void main()
{
    stdio_init_all();
    init_encoder_analog(28); 
    char c ; 
    while (1)
    {
        c = getchar() ; 
        switch (c)
        {
            case 'a': 
                printf("leyendo lecturas de adc dato crudo\r\n"); 
                processing_and_send_samples() ; 
                break;
            case 'r': 
                printf("leyendo lecturas de adc dato crudo\r\n"); 
                sample_reference() ; 
                break;
            case 'Z':       
                setZero() ; 
            case 'z':       
                set90();     
                break;
            case 'b':
                printf("obtain angle") ; 
                read_angle() ; 
            default:
                break;
        }
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
    }
}