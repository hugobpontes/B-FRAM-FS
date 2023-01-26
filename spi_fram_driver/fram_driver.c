/*
 * fram_driver.c
 *
 *  Created on: 22/01/2023
 *      Author: hugobpontes
 */
#include "fram_driver.h"


#define WREN  0b00000110
#define WRDI  0b00000100
#define RDSR  0b00000101
#define WRSR  0b00000001
#define READ  0b00000011
#define WRITE 0b00000010
#define RDID  0b10011111

void FRAM_Reset_CS()
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
}

void FRAM_Set_CS()
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
}


void get_FRAM_ID(void* data_ptr)
{
	  uint8_t command;

	  FRAM_Reset_CS();

	  command = RDID;
	  HAL_SPI_Transmit(&hspi1, &command, 1, 100);
	  HAL_SPI_Receive(&hspi1, data_ptr, 4, 100);

	  FRAM_Set_CS();
}

void write_FRAM(uint16_t address,uint16_t data_length,void* data_ptr)
{
	uint8_t command;
	uint8_t byte_add[2];
	byte_add[0] = (0xFF00 & address) >> 8;
	byte_add[1] = 0x00FF & address;

	FRAM_Reset_CS();

	command = WREN;
	HAL_SPI_Transmit(&hspi1, &command, 1, 100);

	FRAM_Set_CS();
	FRAM_Reset_CS();

	command = WRITE;
	HAL_SPI_Transmit(&hspi1, &command, 1, 100);
	HAL_SPI_Transmit(&hspi1, byte_add, 2, 100);
	HAL_SPI_Transmit(&hspi1, data_ptr, data_length, 100);

	FRAM_Set_CS();
	FRAM_Reset_CS();

	command = WRDI;
	HAL_SPI_Transmit(&hspi1, &command, 1, 100);

	FRAM_Set_CS();
}

void read_FRAM(uint16_t address,uint16_t data_length,void* data_ptr)
{
	uint8_t byte_add[2];
	byte_add[0] = (0xFF00 & address) >> 8;
	byte_add[1] = 0x00FF & address;

	uint8_t command;

	FRAM_Reset_CS();

	command = READ;
	HAL_SPI_Transmit(&hspi1, &command, 1, 100);
	HAL_SPI_Transmit(&hspi1, byte_add, 2, 100);
	HAL_SPI_Receive(&hspi1, data_ptr, data_length, 100);

	FRAM_Set_CS();
}
