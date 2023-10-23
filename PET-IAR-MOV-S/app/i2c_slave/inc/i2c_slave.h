#include "pico/stdlib.h"
#define BUFFER_TX 4 
#define BUFFER_RX 4
//#define SLAVE_I2C 0x42      // Direccion esclavo horizontal
#define SLAVE_I2C 0x48      // Direccion esclavo vertical
#define CLK_SPEED 100000 

typedef void (*prt_dma_rx)(uint8_t *) ; 
typedef void (*prt_dma_tx)(uint16_t *) ; 

/**
 * @brief Inicializa el puerto i2c0 para la plataforma RP2040 
 * 
 * @param port_a : VERR LA HOJA DE DATOS DEL UC RP2040 
 * @param port_b : VERR LA HOJA DE DATOS DEL UC RP2040 
 */
void init_I2C(uint16_t port_a, uint16_t port_b,prt_dma_rx dma_user_rx,prt_dma_tx dma_user_tx ) ; 

