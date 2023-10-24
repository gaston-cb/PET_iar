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

//#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/gpio.h"
#include <hardware/clocks.h>
#include <hardware/timer.h>
#include "pwm_control.h"
#include "encoder_analog.h"
#include "pid_digital.h"
#include "i2c_slave.h"
#include "mode_management.h"

/*================ DEFINICIONES ==============================================================*/
#define PORTS_I2C_SDA 4u // I2C SDA GPIO 4 (pico pin 6)
#define PORTS_I2C_SCL 5u // I2C SCL GPIO 5 (pico pin 7)
#define LPWM_PIN 16u     // LEFT PWM GPIO 16 (pico pin 21)
#define RPWM_PIN 18u     // RIGHT PWM GPIO 18 (pico pin 24)
#define FINAL_B_PIN 20u  // Final de carrera AH GPIO 27 (pico pin 32)
#define FINAL_A_PIN 21u // Final de carrera H GPIO 28 (pico pin 34)
/// TIME IN MS
#define SAMPLING_TIME 1 /// TIME SAMPLING IN miliseconds



/// Constantes PID
float kp = 1.0;
float ki = 1.0;
float kd = 0.1;
float set_point = 90;
bool PID_state = false;
encoder_analog_t encoder_ag ; 
static bool new_cmd;
uint8_t fifo_rx[BUFFER_RX];
uint8_t fifo_tx[BUFFER_TX]; /// view size buffer of tx 

typedef struct {
    encoder_analog_t enc ; 
    cfg_pid pid_par      ;  
}cfg_device_t ; 





BTS7960_t bridge_h = {
    RPWM_PIN,
    LPWM_PIN,
    0,
    0,
};

bool marcha_h = false;
bool marcha_ah = false;
char comando;
int16_t cuenta_pwm;

volatile uint16_t counter_test = 0;

// Definicion de funciones

void core1task(void);
bool systick(struct repeating_timer *t);
void motor_stop(void);
void motor_move_ah(uint16_t vel);
void motor_move_h(uint16_t vel);
void cero_encoder(void);

/*================ MAIN CORE 0 ===============================================================*/
void main()
{
    stdio_init_all();
    init_pwm(&bridge_h);                    // Inicio de pwm
    init_encoder_analog(28) ; 
    multicore_launch_core1(core1task);      // START CORE1
    struct repeating_timer timer;           // Timer
    add_repeating_timer_ms(SAMPLING_TIME, &systick, NULL, &timer);
    sleep_ms(2000) ; 
    printf("=================================\r\n");
    printf("----- INICIO ROTADOR - IAR ------\r\n");
    printf("=================================\r\n");
    while (1)
    {
        if (new_cmd == true)        // Si llega un comando por I2C
        {
            new_cmd = false;        
            command_receive(fifo_rx,4) ; 
        }            
        if (counter_test>=100){
            counter_test = 0 ; 
            //get_data_toi2c() ;    
        }
    // alive test device 
//#ifdef TESTING_UART         
//#endif
    }
}
/*================ FUNCIONES CORE 0 ==========================================================*/
/**
 * 
 * @brief   Callback para el timer
 * 
 * @param t variable tipo repeating_timer
 * @return  true
*/
bool systick(struct repeating_timer *t)
{
    //clock_pwm = 1;
    counter_test++;
    return true;
}


/*==================== CORE 1 ====================================================================*/

/**
 * @brief Rutina para la recepcion via I2C
*/
void dma_u1(uint8_t *bufferrx)
{
    /// buffer rx -> data received of dma channel!
    memcpy(fifo_rx, bufferrx, BUFFER_RX);
    new_cmd = true;
}


/**
 * @brief Rutina para el envio via I2C
*/
volatile void dma_u2(uint16_t *buffertx)
{
    // printf("-> Angulo: %0.2f\r\n", enc.angle);
    // buffertx[0] = (uint16_t) 'a';    //getState();
    // buffertx[1] = (uint16_t)((0xFF00 & ((uint16_t)enc.angle)) >> 8);
    // buffertx[2] = (uint16_t)((0x00FF & ((uint16_t)enc.angle)));
    // buffertx[3] = (uint16_t)((enc.angle - (uint16_t)enc.angle) * 100);
}

void core1task(void)
{
    init_I2C(4, 5, dma_u1, dma_u2);

    while (1)
    {
        tight_loop_contents();
    }
}
/*============================================================================================*/
// EOF
