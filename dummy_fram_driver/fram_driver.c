/*
 * fram_driver.c
 *
 *  Created on: 22/01/2023
 *      Author: hugobpontes
 */
#include "fram_driver.h"

FRAM_t FRAM;

/* Driver functions */

void get_FRAM_status(void* data_ptr)
{

}

void write_FRAM(uint16_t address,uint16_t data_length,void* data_ptr)
{
	for(uint16_t idx = 0;idx<data_length;idx++)
	{
		FRAM[address+idx]=*(uint8_t*)(data_ptr+idx);
	}
}

void read_FRAM(uint16_t address,uint16_t data_length,void* data_ptr)
{
	for(uint16_t idx = 0;idx<data_length;idx++)
	{
		*(uint8_t*)(data_ptr+idx)=FRAM[address+idx];
	}
}
