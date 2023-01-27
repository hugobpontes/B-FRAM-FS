/*
 * fram_driver.c
 *
 *  Created on: Whenever you're reading this
 *      Author: Your lovely name
 */
#include "fram_driver.h"




void get_FRAM_ID(void* data_ptr)
{
	***Write here a function that writes the FRAM ID in data_ptr***
}

void write_FRAM(uint16_t address,uint16_t data_length,void* data_ptr)
{
	***Write here a function that takes a uint16_t address, reads data_length bytes at data_ptr, and writes them at the FRAM location specified by address***
}

void read_FRAM(uint16_t address,uint16_t data_length,void* data_ptr)
{
	***Write here a function that takes a uint16_t address, reads data_length bytes at the FRAM location specified by address, and writes them in data_ptr***
}
