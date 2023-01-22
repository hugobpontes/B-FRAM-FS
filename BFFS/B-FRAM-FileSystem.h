/*
 * B-FRAM-FileSystem.h
 *
 *  Created on: 22/01/2023
 *      Author: Utilizador
 */

#ifndef INC_B_FRAM_FILESYSTEM_H_
#define INC_B_FRAM_FILESYSTEM_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "fram_driver.h"

#define MAX_FILES	20 //Max allowed files on the file system
#define MAX_FILENAME_SIZE 10

#define FILE_SIZE 8+(MAX_FILENAME_SIZE) //Size in bytes of a file struct

#define FS_SIZE ((FILE_SIZE)*(MAX_FILES))+8 //Size in bytes taken by one instance of BFFS
#define FS_OFFSET FS_SIZE //FRAM address where data starts being stored

#define USABLE_SIZE (FRAM_SIZE) - (FS_SIZE) //Bytes of FRAM that can be used to store data



/*Enumeration to define all possible mount options for the fs_mount function*/
typedef enum
{
    FS_MOUNT_OPTION_RESET,
	FS_MOUNT_OPTION_LOAD,
}
	bffs_mount_option;

/*Enumeration to define all possible mount options for the read_file function*/
typedef enum
{
	READ_FILE_RESET_READ_PTR,
	READ_FILE_RESET_DONT_READ_PTR,
}
	bffs_read_file_option;

/*Enumeration to define all return statuses for the BFFS functions that don't return data*/
typedef enum
{
    CREATE_FILE_SUCCESS,
	CREATE_FILE_BAD_FILENAME,
	CREATE_FILE_BAD_SIZE,
	CREATE_FILE_FILE_TOO_LARGE,
	CREATE_FILE_FILENAME_TAKEN,
	CREATE_FILE_INVALID_FILE_PTR,
	CREATE_FILE_NO_FILE_SLOTS,
	//
    MOUNT_FS_SUCCESS,
	MOUNT_FS_NO_MEMORY,
	MOUNT_FS_BAD_OPTION,
	MOUNT_FS_INVALID_FS,
	//
    WRITE_FILE_SUCCESS,
	WRITE_FILE_OVERFLOW,
	WRITE_FILE_INVALID_FILE_PTR,
    WRITE_FILE_INVALID_DATA_PTR,
	WRITE_FILE_BAD_LENGTH,
	//
    READ_FILE_SUCCESS,
	READ_FILE_OVERFLOW,
	READ_FILE_INVALID_FILE_PTR,
    READ_FILE_INVALID_DATA_PTR,
	READ_FILE_BAD_LENGTH,
	//
	OPEN_FILE_SUCCESS,
	OPEN_FILE_FILE_NOT_FOUND,
	OPEN_FILE_INVALID_FILE_PTR,
	//
	CLEAR_FILE_SUCCESS,
	CLEAR_FILE_INVALID_FILE_PTR,
	//
	SEEK_FILE_SUCCESS,
	SEEK_FILE_INVALID_FILE_PTR,
	SEEK_FILE_OVERFLOW,
	//
	SAVE_FS_SUCCESS,
} bffs_st;

//Pointers refer to the fram address, not the the byte within a file

typedef struct file
{

  char filename[MAX_FILENAME_SIZE];
  uint16_t read_ptr;
  uint16_t write_ptr;
  uint16_t start_ptr;
  uint16_t end_ptr;
} file_t;

typedef struct file_system
{
  file_t files[MAX_FILES];
  uint16_t file_idx;
  uint16_t write_ptr;
  uint16_t end_ptr;
  uint16_t start_ptr;

} file_system_t;


bffs_st save_fs();
bffs_st load_fs();
bffs_st reset_fs(uint16_t fs_size);
bffs_st mount_fs(uint16_t fs_size, bffs_mount_option option);
bffs_st create_file(char* filename, uint16_t file_size, file_t** file_ptr_ptr);
bffs_st open_file(char* filename,file_t** file_ptr_ptr);
uint16_t write_file(file_t* file_ptr, uint16_t data_length, void* data_ptr);
bffs_st read_file(file_t* file_ptr, uint16_t data_length, void* data_ptr, bffs_read_file_option option);
bffs_st clear_file(file_t* file_ptr);
bffs_st seek_file(file_t* file_ptr, uint16_t byte);
uint16_t tell_file(file_t* file_ptr);
uint16_t get_fs_free_bytes(void);
uint16_t get_fs_size(void);
uint16_t get_fs_free_file_slots(void);
uint16_t get_fs_total_file_slots(void);
uint16_t get_fs_total_files(void);
uint16_t get_file_free_bytes(file_t* file_ptr);
uint16_t get_file_used_bytes(file_t* file_ptr);
uint16_t get_file_size(file_t* file_ptr);

extern file_system_t BFFS;


#endif /* INC_B_FRAM_FILESYSTEM_H_ */
