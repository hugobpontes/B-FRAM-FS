/*
 * dummy_fram_driver.h
 *
 *  Created on: 22/01/2023
 *      Author: Utilizador
 */

#ifndef INC_FRAM_DRIVER_H_
#define INC_FRAM_DRIVER_H_

#define FRAM_SIZE 8192

#include <stdint.h>

typedef char FRAM_t[FRAM_SIZE];

void get_FRAM_status(void* data_ptr);
void write_FRAM(uint16_t address,uint16_t data_length,void* data_ptr);
void read_FRAM(uint16_t address,uint16_t data_length,void* data_ptr);

#endif /* INC_DUMMY_FRAM_DRIVER_H_ */
