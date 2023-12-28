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
#include "pico/stdlib.h"
#include "encoder_analog.h"
/*================ DEFINICIONES ==============================================================*/
encoder_quad_t enc_test;
uint16_t raw_data_analog[2000] ; 

void processing_and_send_samples() ; 
/*================ MAIN CORE 0 ===============================================================*/
void main()
{
    stdio_init_all();
    init_encoder_analog(28); 
    uint16_t i = 0 ; 
    char c ; 
    while (1)
    {
        c = getchar() ; 
        switch (c)
        {
        case 'a': 
            printf("espere mientras se procesan las muestras \r\n"); 
            processing_and_send_samples() ; 
            break;
        case 'b': 
            break ; 
        default:
            break;
        }



        //getData(&enc_test) ; 
        //i++ ; 
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
        printf("%d,%02x,%d\r\n",i,raw_data_analog[i],raw_data_analog[i]) ; 

    }
    
    printf("final de archivo"); 




}