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
#include "limit_switch.h"
/*================ DEFINICIONES ==============================================================*/
#define PORTS_I2C_SDA 4u // I2C SDA GPIO 4 (pico pin 6)
#define PORTS_I2C_SCL 5u // I2C SCL GPIO 5 (pico pin 7)
#define LPWM_PIN 16u     // LEFT PWM GPIO 16 (pico pin 21)
#define RPWM_PIN 18u     // RIGHT PWM GPIO 18 (pico pin 24)
#define FINAL_B_PIN 20u  // Final de carrera AH GPIO 27 (pico pin 32)
#define FINAL_A_PIN 21u // Final de carrera H GPIO 28 (pico pin 34)
/// TIME IN MS
#define SAMPLING_TIME 1 /// TIME SAMPLING IN miliseconds
typedef enum{
    CALIBRATION, 
    MANUAL, 
    AUTOMATIC,
    ENGINEER, 
}use_mode_t; 



/// Constantes PID
float kp = 1.0;
float ki = 1.0;
float kd = 0.1;
float set_point = 90;
bool PID_state = false;
encoder_analog_t encoder_ag ; 
static bool new_cmd;
uint8_t fifo_rx[BUFFER_RX];
uint8_t fifo_tx[BUFFER_TX];

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
    init_switch(FINAL_A_PIN,FINAL_B_PIN) ; 
    multicore_launch_core1(core1task);      // START CORE1
    struct repeating_timer timer;           // Timer
    add_repeating_timer_ms(SAMPLING_TIME, &systick, NULL, &timer);
    sleep_ms(2000) ; 
    printf("=================================\r\n");
    printf("----- INICIO ROTADOR - IAR ------\r\n");
    printf("=================================\r\n");
    float angle_set ; 
    while (1)
    {
        if (new_cmd == true)        // Si llega un comando por I2C
        {
            new_cmd = false;        
            switch ((char)fifo_rx[0]){
                case 'a':       // Movimiento manual antihorario
                    if (isSwitchOn() == FC_AH){ // Si esta al fin del mov antihorario
                        motor_stop();
                    }else{ // Arranco antihorario
                        cuenta_pwm = fifo_rx[1] * 256 + fifo_rx[2]; // Calculo el pwm
                        motor_move_ah(cuenta_pwm);                  // Muevo el motor
                        printf("\r\n-> Movimiento manual en sentido AH\r\n");
                    }
                    break;
                case 'h':       // Movimiento manual horario
                    if (isSwitchOn() == FC_H){ // Si esta al fin del mov horario
                        motor_stop();
                    }else{ // Arranco horario
                        cuenta_pwm = fifo_rx[1] * 256 + fifo_rx[2];
                        motor_move_h(cuenta_pwm);
                        printf("\r\n-> Movimiento manual en sentido H\r\n");
                    }
                    break;
                case 's':       // Stop
                    motor_stop();
                    PID_state = false;
                    break;
                case 'z':       // Buscar el cero del encoder
                    cero_encoder();
                    break;
                case 'u':       // Actualizo el Set Point
                    set_point = (fifo_rx[1] * 256 + fifo_rx[2]) / 100.0;
                    printf("\r\n-> Set point: %0.2f\r\n",set_point);
                    break;
                case 'p':       // Actualizo el Kp
                    kp = (fifo_rx[1] * 256 + fifo_rx[2]) / 100.0;
                    setttings_pid(kp,ki,kd);
                    printf("\r\n-> Kp: %0.2f\r\n",kp);
                    break;
                case 'i':       // Actualizo el Ki
                    ki = (fifo_rx[1] * 256 + fifo_rx[2]) / 100.0;
                    setttings_pid(kp,ki,kd);
                    printf("\r\n-> Ki: %0.2f\r\n",ki);
                    break;
                case 'd':       // Actualizo el Kd
                    kd = (fifo_rx[1] * 256 + fifo_rx[2]) / 100.0;
                    setttings_pid(kp,ki,kd);
                    printf("\r\n-> Kd: %0.2f\r\n",kd);
                    break;
                case 'o':       // PID ON
                    printf("\r\n-> PID ON\r\n");
                    PID_state = true;
                    counter_test = 0;
                    break;
                case '\n':
                case '\r':
                case 'g':       //Comando para enviar un angulo
                    break;
                case 'l':
                    angle_set = (fifo_rx[1] * 256 + fifo_rx[2]) / 100.0; ///positional arguments  
                    set_angle(angle_set) ; 
                    break ; 
                default:
                    printf("\r\nCommand desconocido: %c\r\n", (char)fifo_rx[0]);
                    break;
                }
        }
        // Control de STOP
        if (marcha_h == true && isSwitchOn() == FC_H){
//            printf("end of ")
            motor_stop();
        }
        if (marcha_ah == true && isSwitchOn() == FC_AH){
            motor_stop();
        }
        // PID ON
        if (PID_state == true && counter_test >= 100){
            compute_pid(set_point,(float)(counter_test / 1000));
            counter_test = 0;
        }
//      Alive test
//      get_sample(&encoder_ag) ; 
//      printf("an-value: %d -angle: %.3f \r\n", encoder_ag.angle_read, encoder_ag.angle) ; 
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

/**
 * @brief Detiene el movimiento del motor
 */
void motor_stop(void)
{
    bridge_h.percent_l = 0; /// quieto
    bridge_h.percent_r = 0; /// quieto
    set_pwm(&bridge_h);
    marcha_h = false;
    marcha_ah = false;
    //printf("\r\n-> STOP\r\n");
    //getData(&enc_test);
    //printf("-> Angulo: %0.2f \r\n", enc_test.angle);
    
    //printf("-> Pulsos: %d \r\n",enc_test.count_pulses);
}

/**
 * @brief Movimiento del motor en sentido antihorario
 *
 * @param vel: velocidad del motor, de 0 a TOP_VALUE_COUNT
 */
void motor_move_ah(uint16_t vel)
{
    bridge_h.percent_l = vel; /// sentido antihorario
    bridge_h.percent_r = 0;   /// sentido antihorario
    set_pwm(&bridge_h);
    marcha_ah = true;
}

/**
 * @brief Movimiento del motor en sentido horario
 *
 * @param vel: velocidad del motor, de 0 a TOP_VALUE_COUNT
 */
void motor_move_h(uint16_t vel)
{
    bridge_h.percent_l = 0;   /// sentido horario
    bridge_h.percent_r = vel; /// sentido horario
    set_pwm(&bridge_h);
    marcha_h = true;
}

/**
 * @brief Busca el final de carrera del movimiento antihorario para ajustar el cero del encoder
 */
void cero_encoder(void)
{
    if (isSwitchOn() != FC_AH)
    { // Si no estoy al final de AH
        printf("-> Buscando la posicion inicial...\r\n");
        motor_move_h((uint16_t) (0.75 * TOP_VALUE_COUNT)); // Me muevo AH a un 25% de VMAX
        while (isSwitchOn() != FC_AH); // Espero a llegar al final de AH
        sleep_ms(100) ; 
        motor_stop()  ;
    }
    set_zero(); 


}


/*================ CORE 1 ====================================================================*/

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
