# Basic FRAM File System

BFFS is a very basic implementation of a file system to be used on an FRAM (Ferroelectric Random Access Memory). I made (am making) because there aren't a lot of FRAM file systems available out there - probably because using one with FRAM isn't the most common option - and because I wanted to learn more about file systems and to have fun making one, and I have! This isn't the most complex or efficient file system out there but it provides most basic functionalities one needs to manage files in an embedded system, and is very begginer friendly. With that in mind, I've tried to document it as well as possible.
I've tested this file system on an adafruit MB85RS64V 64k bit SPI FRAM using an STM32 F767ZI microcontroller and consequently had to write a driver for it. This driver is included and you must adapt the underlying functions to your microcontroller and interfaces if they aren't the same.
I've also included a dummy driver that emulates FRAM as a uint8_t array just to test the file system functions, which is also included.

## Contents
BFFS: File system source and header file
Examples: Examples of programs that use this file system and drivers
Dummy FRAM Driver: Dummy driver that includes software for interaction with an emulated FRAM (as a uint8_t array) just to test the file system functions.
SPI FRAM Driver: Driver that includes the software for interaction with the selected FRAM using SPI.

## Features

The functions that the file system provides are: (fs meaning file system)
```
save_fs();
load_fs();
reset_fs(uint16_t fs_size);
mount_fs(uint16_t fs_size, bffs_mount_option option);
create_file(char* filename, uint16_t file_size, file_t** file_ptr_ptr);
open_file(char* filename,file_t** file_ptr_ptr);
write_file(file_t* file_ptr, uint16_t data_length, void* data_ptr);
read_file(file_t* file_ptr, uint16_t data_length, void* data_ptr, bffs_read_file_option option);
clear_file(file_t* file_ptr);
seek_file(file_t* file_ptr, uint16_t byte);
tell_file(file_t* file_ptr);
get_fs_free_bytes(void);
get_fs_size(void);
get_fs_free_file_slots(void);
get_fs_total_file_slots(void);
get_fs_total_files(void);
get_file_free_bytes(file_t* file_ptr);
get_file_used_bytes(file_t* file_ptr);
get_file_size(file_t* file_ptr);
```
The functions that the FRAM driver provides are
```
get_FRAM_status(void* data_ptr);
write_FRAM(uint16_t address,uint16_t data_length,void* data_ptr);
read_FRAM(uint16_t address,uint16_t data_length,void* data_ptr);
```
## Limitations
This file system provides no way to delete files and no way to list all files in the file system, which means that if you reach the maximum file limit, you will have to reset the whole file system in order to write more data; Additionally, if you load a FS from the FRAM, you won't know what files are written there, unless you were the one to put them there.
 
## To-do
-Add a way of deleting files and managing the FRAM accordingly so memory is actually freed without the need to reset the file system.

-Add a list files function.

## Usage
Just include ```B-FRAM-FileSystem.h``` and ```fram_driver.h``` in your main application source file and use it as shown in the examples folder. If you wish, you can also alter some parameters like max files in the ```B-FRAM-FileSystem.h``` file. Don't forget that to use BFFS for a different microcontroller or FRAM you will have to change the driver accordingly. If you do, please fork this repo and request a pull after you've implemented it, the more drivers, the merrier! 


