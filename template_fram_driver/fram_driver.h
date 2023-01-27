/*
 * fram_driver.h
 *
 *  Created on: Whenever you're reading this
 *      Author: Your lovely name
 */
#ifndef INC_FRAM_DRIVER_H_
#define INC_FRAM_DRIVER_H_

#define FRAM_SIZE ***INSERT THE SIZE OF YOUR FRAM HERE*** //Sizes in bytes of the FRAM

#include <stdint.h>

***Include the header files for your peripherals***

***Declare your peripherals handlers*** 

void get_FRAM_ID(void* data_ptr);
void write_FRAM(uint16_t address,uint16_t data_length,void* data_ptr);
void read_FRAM(uint16_t address,uint16_t data_length,void* data_ptr);

#endif /* INC_DUMMY_FRAM_DRIVER_H_ */
