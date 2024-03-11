/*
 * portsESP32.c
 *
 *  Created on: Jun 7, 2022
 *      Author: Gaston Valdez
 *      Organization: Instituto Argentino de radioastronomia
 */
/*
 * portsESP32.c
 *
 *  Created on: Jun 7, 2022
 *      Author: root
 */
#include <stdint.h>
#include "portsRP2040.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"

static uint8_t init_i2c_status = false ; 


static void init_i2c(void){ 
	i2c_init(i2c1,400000) ; 
    gpio_set_function(27,GPIO_FUNC_I2C ) ; 
    gpio_set_function(26,GPIO_FUNC_I2C ) ; 
	gpio_pull_up(27) ; 
    gpio_pull_up(26) ; 
	init_i2c_status = true ; 
}


/**
 *
 * @param I2C_address
 * @param buffer_data
 * @param length_buffer
 * @return error code: Ox01 -> success
 * 					 : OxFF -> error
 *
 */
uint8_t  I2CWriteToSlave(uint8_t I2C_address, uint8_t *buffer_data,size_t length_buffer){
	uint8_t response ; 
	if (init_i2c_status == false){
		init_i2c() ; 
	}
	response = i2c_write_timeout_us(i2c1, 
						I2C_address,
						buffer_data,
						length_buffer,
						false, 
						(uint )10000 ) ; 

	if (response == PICO_ERROR_GENERIC || response == PICO_ERROR_TIMEOUT){
		response =ERROR_I2C_FAIL ; 
	}else{
		response =ERROR_I2C_OK ; 

	}
	return  response  ;

}

/**
 *
 * @param I2C_address
 * @param buffer_data
 * @param length_buffer
 */

uint8_t I2CReadToSlave(uint8_t I2C_address, uint8_t *buffer_data,size_t length_buffer){
	uint8_t response; 
	if (init_i2c_status == false){
		init_i2c() ; 
	}
	response = i2c_read_timeout_us(i2c1, 
						I2C_address,
						buffer_data,
						length_buffer,
						false, 
						(uint )10000 ) ; 

	if (response == PICO_ERROR_GENERIC || response == PICO_ERROR_TIMEOUT){
		response =ERROR_I2C_FAIL ; 
	}else{
		response =ERROR_I2C_OK ; 

	}
	return response ; 
}





