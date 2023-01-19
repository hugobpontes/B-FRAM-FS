/*
 ============================================================================
 Name        : B-FRAM-FileSystem.c
 Author      : hugobpontes
 Version     :
 Copyright   : Your copyright notice
 Description : Basic structure of future file system applications
 ============================================================================
 */
#define FRAM_SIZE 8192
#define MAX_FILENAME_SIZE 50
#define MAX_FILES	10

#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef enum
{
    CREATE_FILE_SUCCESS,
	CREATE_FILE_BAD_FILENAME,
	CREATE_FILE_BAD_SIZE,
	CREATE_FILE_FILE_TOO_LARGE,
	//
    MOUNT_FS_SUCCESS,
	MOUNT_FS_NO_MEMORY,
	//
    WRITE_FILE_SUCCESS,
	WRITE_FILE_OVERFLOW,
	//
    READ_FILE_SUCCESS,
	READ_FILE_OVERFLOW,
} bffs_st;

typedef char FRAM_t[FRAM_SIZE];

typedef struct file
{
  char filename[MAX_FILENAME_SIZE];
  uint16_t read_ptr;
  uint16_t write_ptr;
  uint16_t start_ptr;
  uint16_t end_ptr;
  //uint8_t state (deleted or not)
} file_t;

typedef struct file_system
{
  file_t files[MAX_FILES];
  uint16_t file_idx;
  uint16_t write_ptr;
  uint16_t free_bytes ;
} file_system_t;


FRAM_t FRAM;
file_system_t BFFS; //Put this in header file


bffs_st mount_fs(uint16_t fs_size)
{
	  if (fs_size > FRAM_SIZE)
	  {
		  return MOUNT_FS_NO_MEMORY;
	  }
	  BFFS.file_idx = 0;
	  BFFS.write_ptr = 0;
	  BFFS.free_bytes = fs_size;
	  return MOUNT_FS_SUCCESS;
}

bffs_st create_file(char* filename, uint16_t file_size, file_t** file_ptr_ptr)
{

	/*Set filename and return error if too long*/
	for (uint8_t idx = 0; *(filename+idx) != '\0'; idx++)
	{
		BFFS.files[BFFS.file_idx].filename[idx]= *(filename+idx);
		if (idx == MAX_FILENAME_SIZE)
		{
			return CREATE_FILE_BAD_FILENAME;
		}
	}
	/*Check file size in not 0 and return error if it is*/
	if (!file_size)
	{
		return CREATE_FILE_BAD_SIZE;
	}
	if ((file_size + BFFS.write_ptr) > FRAM_SIZE)
	{
		return CREATE_FILE_FILE_TOO_LARGE;
	}
	//Set pointers
	BFFS.files[BFFS.file_idx].start_ptr = BFFS.write_ptr;
	BFFS.files[BFFS.file_idx].end_ptr   = BFFS.write_ptr + file_size;
	BFFS.files[BFFS.file_idx].write_ptr = BFFS.write_ptr;
	BFFS.files[BFFS.file_idx].read_ptr   = BFFS.write_ptr;

	BFFS.file_idx++;
	BFFS.write_ptr+= file_size;

	*file_ptr_ptr = &(BFFS.files[BFFS.file_idx]);


	return CREATE_FILE_SUCCESS;
}

void open_file(char* filename,file_t* file_ptr)
{
 //return a file pointer based on name
 //considering merging this with create?
}


bffs_st write_file(file_t* file_ptr, uint16_t data_length, void* data_ptr)
{
	uint16_t write_end_ptr = file_ptr->write_ptr+data_length;
	if (write_end_ptr > file_ptr->end_ptr)
		return WRITE_FILE_OVERFLOW;
	for (uint16_t idx = 0; idx < data_length; idx++)
	{
		FRAM[file_ptr->write_ptr]=*(uint8_t*)(data_ptr+idx);//replace this with write_byte_FRAM function so that this is loosely coupled
		file_ptr->write_ptr++;
	}
	return WRITE_FILE_SUCCESS;
}

bffs_st read_file(file_t* file_ptr, uint16_t data_length, void* data_ptr)
{
	uint16_t read_end_ptr = file_ptr->read_ptr+data_length;
	if (read_end_ptr > file_ptr->end_ptr)
		return READ_FILE_OVERFLOW;
	for (uint16_t idx = 0; idx < data_length; idx++)
	{
		*(uint8_t*)(data_ptr+idx) = FRAM[file_ptr->write_ptr];//replace this with read_byte_FRAM function so that this is loosely coupled
		file_ptr->write_ptr++;
	}
	file_ptr->read_ptr = 0;
	return READ_FILE_SUCCESS;
}


//clear file, reset write pointer and write all 0s

int main(void)
{
	bffs_st status;
	file_t* myfile;

	uint8_t data_w[3] = {10,20,30};
	uint8_t data_r[3];

	char myfilename[10] = "file.txt";
	status = create_file(myfilename,10,&myfile);
	status = write_file(myfile,3,data_w);
	status = read_file(myfile,3,data_r);

	printf("Data Read: [%d,%d,%d]\n",data_r[0],data_r[1],data_r[2]);

}
