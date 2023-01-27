/*
 * fram_driver.c
 *
 * Valid for a STM32F767ZI microcontroller and MB85RS64V 64k bit SPI FRAM 
 *
 *  Created on: 22/01/2023
 *      Author: hugobpontes
 */
#include "fram_driver.h"


#define WREN  0b00000110 //Write Enable
#define WRDI  0b00000100 //Wreite Disable
#define RDSR  0b00000101 //Read Status Register
#define WRSR  0b00000001 //Write Status Register
#define READ  0b00000011 //Read Data
#define WRITE 0b00000010 //Write Data
#define RDID  0b10011111 //Read Device ID

void FRAM_Reset_CS()
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
}

void FRAM_Set_CS()
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
}

/*Function that writes the FRAM ID in data_ptr by
	1. resetting the FRAM SPI CS pin
	2. sending the RDID command via SPI
	3. receiving 4 bytes of response via SPI
	4. setting the FRAM SPI CS pin */
void get_FRAM_ID(void* data_ptr)
{
	  uint8_t command;

	  FRAM_Reset_CS();

	  command = RDID;
	  HAL_SPI_Transmit(&hspi1, &command, 1, 100);
	  HAL_SPI_Receive(&hspi1, data_ptr, 4, 100);

	  FRAM_Set_CS();
}

/*Function that takes a uint16_t address, reads data_length bytes at data_ptr, and writes them at the FRAM location specified by address by:
	1. converting the uint16_t address into a 2 uint8_t array
	2. resetting the FRAM SPI CS pin
	3. sending the WREN command via SPI
	4. Setting and resetting the CS pin
	5. sending the WRITE command via SPI
	6. sending data_length bytes of data via SPI
	7. Setting and resetting the CS pin
	8. sending the WRDI command via SPI
	9. setting the FRAM SPI CS pin */
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

/*Function that takes a uint16_t address, reads data_length bytes at the FRAM location specified by address, and writes them in data_ptr by:
	1. converting the uint16_t address into a 2 uint8_t array
	2. resetting the FRAM SPI CS pin
	3. sending the READ command via SPI
	4. receiving data_length bytes of data via SPI
	5. setting the FRAM SPI CS pin */
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
