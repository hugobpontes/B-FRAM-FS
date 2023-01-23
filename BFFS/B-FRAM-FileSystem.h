/*
 * B-FRAM-FileSystem.h
 *
 *  Created on: 22/01/2023
 *      Author: hugobpontes
 */

#ifndef INC_B_FRAM_FILESYSTEM_H_
#define INC_B_FRAM_FILESYSTEM_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "fram_driver.h"

#define MAX_FILES	20 //Max allowed files that can be stored in the file system
#define MAX_FILENAME_SIZE 10

#define FILE_STRCT_SIZE 8+(MAX_FILENAME_SIZE) //Size in bytes of a file struct

#define FS_STRCT_SIZE ((FILE_STRCT_SIZE)*(MAX_FILES))+8 //Size in bytes taken by one instance of BFFS
#define FS_OFFSET FS_STRCT_SIZE //FRAM address where data starts being stored

#define USABLE_SIZE (FRAM_SIZE) - (FS_STRCT_SIZE) //Bytes of FRAM that can be used to store data



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

/*File: pointers refer to the fram address, not the the byte within a file. Start and end pointers define where in
 * FRAM the file data stars and ends, while the others define where data is being written and read
 */
typedef struct file
{

  char filename[MAX_FILENAME_SIZE];
  uint16_t read_ptr;
  uint16_t write_ptr;
  uint16_t start_ptr;
  uint16_t end_ptr;
} file_t;

/*File System: pointers refer to the fram address, not the the byte within a file. Start and end pointers define where in
 * FRAM the file data can be stored, while write ptr defines where new created file's data is being stored in.
 */
typedef struct file_system
{
  file_t files[MAX_FILES];
  uint16_t file_idx;
  uint16_t write_ptr;
  uint16_t end_ptr;
  uint16_t start_ptr;

} file_system_t;


bffs_st save_fs();
/*******************************************************************
* NAME :            save_fs
*
* DESCRIPTION :     Save file system struct at the beginning of FRAM
*
* INPUTS :
*       PARAMETERS:

*       GLOBALS :
*           file_system_t BFFS: File System Handle
*           #define		  FS_STRCT_SIZE: Macro defining the size of the file system struct
* OUTPUTS :
*       PARAMETERS:
*       GLOBALS :
*       RETURN :
*          bffs_st status: Status of the operation
* PROCESS :
*                   [1]  Write FS struct in start of FRAM
*
*/
bffs_st load_fs();
/*******************************************************************
* NAME :            load_fs
*
* DESCRIPTION :     Load file system struct from the beginning of FRAM
*
* INPUTS :
*       PARAMETERS:

*       GLOBALS :
*           #define		  FS_STRCT_SIZE: Macro defining the size of the file system struct
*           #define		  FRAM_SIZE: Total size of the FRAM in bytes
*           #define		  MAX_FILES: Maximum files that can be stored in the file system
*
* OUTPUTS :
*       PARAMETERS:
*       GLOBALS :
*           file_system_t BFFS: File System Handle
*       RETURN :
*          bffs_st status: Status of the operation
* PROCESS :
*                   [1] Load FS struct from start of FRAM
*
*/
bffs_st reset_fs(uint16_t fs_size);
/*******************************************************************
* NAME :            reset_fs
*
* DESCRIPTION :     reset the file system to a clean state by resetting its pointers and file slot index
*
* INPUTS :
*       PARAMETERS:
*			uint16_t 	  fs_size: size in bytes that the user wants to allocate for the file system
*       GLOBALS :
*			#define		  FS_OFFSET: FRAM byte in which the file system can start storing data
*
* OUTPUTS :
*       PARAMETERS:
*       GLOBALS :
*           file_system_t BFFS: File System Handle
*       RETURN :
*          bffs_st status: Status of the operation
* PROCESS :
*                   [1] Check if input file system size fits inside FRAM
*                   [2] Reset file system: file idx; start pointer; end pointer; write pointer
*                   [3] Save FS struct in FRAM for loading at a future time
*
*/
bffs_st mount_fs(uint16_t fs_size, bffs_mount_option option);
/*******************************************************************
* NAME :           mount_fs
*
* DESCRIPTION :     either reset the fs struct into a clear state or load a fs struct from FRAM
*
* INPUTS :
*       PARAMETERS:
*			uint16_t 	  		fs_size: size in bytes that the user wants to allocate for the file system
*			bffs_mount_option 	option: option to select whether to load or reset the file system (see typedef enum)
*       GLOBALS :
* OUTPUTS :
*       PARAMETERS:
*       GLOBALS :
*           file_system_t BFFS: File System Handle
*       RETURN :
*          bffs_st status: Status of the operation
* PROCESS :
*                   [1] Check option to determine whether to reset or load
*                   [2] Reset (reset_fs) or load (load_fs)
*
*/
bffs_st create_file(char* filename, uint16_t file_size, file_t** file_ptr_ptr);
/*******************************************************************
* NAME :           create_file
*
* DESCRIPTION :     allocate a user specified amount of bytes to a given file, give it a name, and get a pointer to it
*
* INPUTS :
*       PARAMETERS:
*			char* 				filename: string by which the user can identify the file later
*			uint16_t			file_size: number of bytes of file data to allocate to a given file
*       GLOBALS :
*       	#define				MAX_FILES: Maximum files that can be stored in the file system
*       	#define				MAX_FILENAME_SIZE: Maximum number of chars that a filename can have
* OUTPUTS :
*       PARAMETERS
*       	file_t** 			file_ptr_ptr: pointer to the file pointer that will point to the file struct containing file fields
*       GLOBALS :
*           file_system_t BFFS: File System Handle
*       RETURN :
*          bffs_st status: Status of the operation
* PROCESS :
*                   [1] Check for invalid inputs given BFFS state
*                   [2] Set filename
*                   [3] Set file pointers
*                   [4] Assign file pointer that points to file within BFFS to input variable
*                   [5] Set BFSS pointers
*                   [6] Save FS struct in FRAM for loading at a future time
*
*/
bffs_st open_file(char* filename,file_t** file_ptr_ptr);
/*******************************************************************
* NAME :           open_file
*
* DESCRIPTION :     get pointer to a file with the specified filename
*
* INPUTS :
*       PARAMETERS:
*			char* 				filename: string by which the user can identify the file later
*       GLOBALS :
*       	#define				MAX_FILES: Maximum files that can be stored in the file system
*       	#define				MAX_FILENAME_SIZE: Maximum number of chars that a filename can have
* OUTPUTS :
*       PARAMETERS
*       	file_t** 			file_ptr_ptr: pointer to the file pointer that will point to the file struct containing file fields
*       GLOBALS :
*           file_system_t BFFS: File System Handle
*       RETURN :
*          bffs_st status: Status of the operation
* PROCESS :
*                   [1] Check for invalid inputs given BFFS state
*                   [2] Compare input string to filenames in the BFFS
*                   [3] If a file with a matching file name is found, make the input pointer point to it.
*
*/
uint16_t write_file(file_t* file_ptr, uint16_t data_length, void* data_ptr);
/*******************************************************************
* NAME :           write_file
*
* DESCRIPTION :     copy a given amount of bytes to FRAM location pointed by file, from a given location
*
* INPUTS :
*       PARAMETERS:
*			file_t*				file_ptr: pointer to file struct from which write pointer is obtained
*			uint16_t			data_length: amount of bytes to be written
*       GLOBALS :
* OUTPUTS :
*       PARAMETERS
*       	void*  data_ptr: pointer to the data that is to be written
*       GLOBALS :
*           file_system_t BFFS: File System Handle
*       RETURN :
*          bffs_st status: Status of the operation
* PROCESS :
*                   [1] Check for invalid inputs given BFFS state
*                   [2] Write data in the FRAM according to the file pointers in the file struct
*                   [3] Save FS struct in FRAM for loading at a future time
*
*/
bffs_st read_file(file_t* file_ptr, uint16_t data_length, void* data_ptr, bffs_read_file_option option);
/*******************************************************************
* NAME :            read_file
*
* DESCRIPTION :     copy a given amount of bytes to a given location, from a file
*
* INPUTS :
*       PARAMETERS:
*			file_t*					file_ptr: pointer to file struct from which read pointer is obtained
*			uint16_t				data_length: amount of bytes to be read
*			bffs_read_file_option 	option: option to select whether to reset read pointer or not after read
*       GLOBALS :
* OUTPUTS :
*       PARAMETERS
*       	void*  data_ptr: address of memory location to which read data is written
*       GLOBALS :
*           file_system_t BFFS: File System Handle
*       RETURN :
*          bffs_st status: Status of the operation
* PROCESS :
*                   [1] Check for invalid inputs given BFFS state
*                   [2] Read data from the FRAM according to the file pointers in the file struct
*                   [3] Reset read pointer if such option is selected
*
*/
bffs_st clear_file(file_t* file_ptr);
/*******************************************************************
* NAME :            clear_file
*
* DESCRIPTION :     write all 0's to FRAM in location
*
* INPUTS :
*       PARAMETERS:
*			file_t*	file_ptr: pointer to file struct from which start and end pointers are obtained
*       GLOBALS :
* OUTPUTS :
*       PARAMETERS
*       GLOBALS :
*           file_system_t BFFS: File System Handle
*       RETURN :
*          bffs_st status: Status of the operation
* PROCESS :
*                   [1] Check for invalid inputs given BFFS state
*                   [2] Write 0 data in the FRAM according to the file pointers in the file struct
*                   [3] Reset file pointers
*                   [4] Save FS struct in FRAM for loading at a future time
*
*
*/
bffs_st seek_file(file_t* file_ptr, uint16_t byte);
/*******************************************************************
* NAME :            seek_file
*
* DESCRIPTION :     set file read pointer to a specific byte within the file, in relation to the start pointer
*
* INPUTS :
*       PARAMETERS:
*			file_t*	file_ptr: pointer to file struct from which start and end pointers are obtained
*			uint16_t byte:	  byte within file that the read pointer must be changed to
*       GLOBALS :
* OUTPUTS :
*       PARAMETERS
*       GLOBALS :
*           file_system_t BFFS: File System Handle
*       RETURN :
*          bffs_st status: Status of the operation
* PROCESS :
*                   [1] Check for invalid inputs given BFFS state
*                   [2] Set file read pointer
*
*/
uint16_t tell_file(file_t* file_ptr);
/*******************************************************************
* NAME :            tell_file
*
* DESCRIPTION :     return current read pointer in relation to the start of the file
*
* INPUTS :
*       PARAMETERS:
*			file_t*	file_ptr: pointer to file struct from which start and end pointers are obtained
*       GLOBALS :
* OUTPUTS :
*       PARAMETERS
*       GLOBALS :
*           file_system_t BFFS: File System Handle
*       RETURN :
*          uint16_t byte: Byte within file that the write read pointer is currently at
* PROCESS :
*                   [1] Return current read pointer in relation to the start of the file
*
*/

//From now on functions are pretty self explanatory and simple, so i didnt bother putting a header
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
