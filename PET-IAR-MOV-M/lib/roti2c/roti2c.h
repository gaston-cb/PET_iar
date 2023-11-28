#ifndef _ROTI2C_H_
#define _ROTI2C_H_

#include "Arduino.h"
#include "Wire.h"

// Variable para las direcciones de los esclavos
typedef enum {
    ROT_IAR_PE_H_ADDR = 0x42,        // Direccion del esclavo horizontal
    ROT_IAR_PE_V_ADDR = 0x48         // Direccion del esclavo vertical
} rot_iar_pe_addr_t;

typedef enum {
    
    ROT_IAR_REG_HORARIO = 104,      // 'h' - Comando movimiento manual horario
    ROT_IAR_REG_ANTIHOR = 97,       // 'a' - Comando movimiento manual antihorario
    ROT_IAR_REG_STOP = 115,         // 's' - Comando para parar el motor - STOP
    ROT_IAR_REG_SETMAX = 122,       // 'z' - Comando para rutina de cero
    ROT_IAR_REG_SETCERO = 90,       // 'Z' - Comando para setear el cero
    ROT_IAR_REG_ANGULO = 103,       // 'g' - Comando para leer el angulo
    ROT_IAR_REG_TRACK = 116,        // 't' - Comando para comenzar el movimient
    ROT_IAR_REG_SP = 117,           // 'u' - Comando para enviar el set point
    ROT_IAR_REG_PID_ON = 111,       // 'o' - Comando para iniciar el PID
    ROT_IAR_REG_PID_OFF = 120,      // 'x' - Comando para para el PID
    ROT_IAR_REG_KP = 112,           // 'p' - Comando para enviar la cte Kp
    ROT_IAR_REG_KI = 105,           // 'i' - Comando para enviar la cte Ki
    ROT_IAR_REG_KD = 100            // 'd' - Comando para enviar la cte kd
} rot_iar_pe_reg_t;

class rot_iar {
    // Arduino's I2C library
    TwoWire *_i2c;

    // I2C address
    rot_iar_pe_addr_t _i2c_addr;

    public:

    rot_iar(rot_iar_pe_addr_t addr) : _i2c_addr(addr){};
    // Initializes i2c com
    void begin(TwoWire *theWire = &Wire);
    // Enviar datos a las placas esclavo
    void write(rot_iar_pe_reg_t reg, uint16_t *val);
    // Leer datos de las placas escalvo
    void read(rot_iar_pe_reg_t reg, float *val);
};

#endif