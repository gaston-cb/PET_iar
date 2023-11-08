#include "roti2c.h"

void rot_iar::begin(TwoWire *theWire){
    _i2c = theWire;
    _i2c->begin();
}

/**
  * @brief Enviar datos a las placas esclavo.
  * 
  * @param reg : Registro enviado a la placa esclavo
  * @param val : Dato enviado a la placa esclavo
 */
void rot_iar::write(rot_iar_pe_reg_t reg, uint16_t *val) {
    _i2c->beginTransmission(_i2c_addr);
    _i2c->write(reg);                 // Register
    _i2c->write((*val >> 8) & 0xFF);  // Upper 8-bits
    _i2c->write(*val & 0xFF);         // Lower 8-bits
    _i2c->write(0x00);
    _i2c->endTransmission();
}

/**
 * @brief Leer datos de las placas esclavo
 * 
 * @param reg : Registro enviado a la placa esclavo
 * @param val : Valor recepcionado en float
*/
void rot_iar::read(rot_iar_pe_reg_t reg, float *val){
    // _i2c->beginTransmission(_i2c_addr);
    // _i2c->write(reg);  // Register
    // _i2c->write(reg);  // Register
    // _i2c->write(reg);  // Register
    // _i2c->write(reg);  // Register
    // _i2c->endTransmission(false);

    _i2c->requestFrom((uint8_t)_i2c_addr, (uint8_t)4,true);
    if (_i2c->available()) {
        int estado = _i2c->read();   
        *val = (float)((_i2c->read() * 256.0) + _i2c->read() ) + (_i2c->read()  / 100.0);
    }
 
}