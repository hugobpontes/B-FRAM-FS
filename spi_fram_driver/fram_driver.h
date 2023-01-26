/*
 * fram_driver.h
 *
 *  Created on: 22/01/2023
 *      Author: hugobpontes
 */

#ifndef INC_FRAM_DRIVER_H_
#define INC_FRAM_DRIVER_H_

#define FRAM_SIZE 8192 //Sizes in bytes of the FRAM

#include <stdint.h>
#include "stm32f7xx_hal.h"

extern SPI_HandleTypeDef hspi1;

void get_FRAM_ID(void* data_ptr);
void write_FRAM(uint16_t address,uint16_t data_length,void* data_ptr);
void read_FRAM(uint16_t address,uint16_t data_length,void* data_ptr);

#endif /* INC_DUMMY_FRAM_DRIVER_H_ */
