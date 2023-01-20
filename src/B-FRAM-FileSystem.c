/*
 ============================================================================
 Name        : B-FRAM-FileSystem.c
 Author      : hugobpontes
 Version     :
 Copyright   : Your copyright notice
 Description : Basic structure of future file system applications
 ============================================================================
 */
#define MAX_FILES	10
#define FRAM_SIZE 8192
#define MAX_FILENAME_SIZE 10

#define FILE_SIZE 8+(MAX_FILENAME_SIZE*8)

#define FS_SIZE FILE_SIZE*MAX_FILES+4
#define FS_OFFSET FS_SIZE

#define USABLE_SIZE FRAM_SIZE - FS_SIZE




#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef enum
{
    CREATE_FILE_SUCCESS,
	CREATE_FILE_BAD_FILENAME,
	CREATE_FILE_BAD_SIZE,
	CREATE_FILE_FILE_TOO_LARGE,
	CREATE_FILE_FILENAME_TAKEN,
	//
    MOUNT_FS_SUCCESS,
	MOUNT_FS_NO_MEMORY,
	//
    WRITE_FILE_SUCCESS,
	WRITE_FILE_OVERFLOW,
	//
    READ_FILE_SUCCESS,
	READ_FILE_OVERFLOW,
	//
	OPEN_FILE_SUCCESS,
	OPEN_FILE_FILE_NOT_FOUND,
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

} file_system_t;


FRAM_t FRAM;
file_system_t BFFS;


bffs_st mount_fs(uint16_t fs_size)
{
	  if (fs_size > USABLE_SIZE)
	  {
		  return MOUNT_FS_NO_MEMORY;
	  }
	  BFFS.file_idx = 0;
	  BFFS.write_ptr = FS_OFFSET;

	  return MOUNT_FS_SUCCESS;
}

bffs_st create_file(char* filename, uint16_t file_size, file_t** file_ptr_ptr)
{

	//Get filename that is being created and verify its validity
	char temp_str[MAX_FILENAME_SIZE] = {0};
	for (uint8_t idx = 0; *(filename+idx) != '\0'; idx++)
	{
		if (idx == MAX_FILENAME_SIZE)
		{
			return CREATE_FILE_BAD_FILENAME;
		}
		temp_str[idx]= *(filename+idx);
	}

	//Compare it with existing filenames
	for (uint16_t search_idx =0; search_idx<MAX_FILES; search_idx++)
	{
		if (!strcmp(temp_str,BFFS.files[search_idx].filename))
		{
			return CREATE_FILE_FILENAME_TAKEN;
		}
	}
	/*Check file size is not 0 and return error if it is*/
	if (!file_size)
	{
		return CREATE_FILE_BAD_SIZE;
	}
	/*Check file size is not too large*/
	if ((file_size + BFFS.write_ptr) > USABLE_SIZE)
	{
		return CREATE_FILE_FILE_TOO_LARGE;
	}
	/*Set filename*/
	strcpy(BFFS.files[BFFS.file_idx].filename,temp_str);


	//Set pointers
	BFFS.files[BFFS.file_idx].start_ptr = BFFS.write_ptr;
	BFFS.files[BFFS.file_idx].end_ptr   = BFFS.write_ptr + file_size;
	BFFS.files[BFFS.file_idx].write_ptr = BFFS.write_ptr;
	BFFS.files[BFFS.file_idx].read_ptr  = BFFS.write_ptr;

	*file_ptr_ptr = &(BFFS.files[BFFS.file_idx]);

	BFFS.file_idx++;
	BFFS.write_ptr+= file_size;

	return CREATE_FILE_SUCCESS;
}

bffs_st open_file(char* filename,file_t** file_ptr_ptr)
{
	//Get string that is being searched
	char temp_str[MAX_FILENAME_SIZE] = {0};
	for (uint8_t idx = 0; *(filename+idx) != '\0'; idx++)
	{
		temp_str[idx]= *(filename+idx);
	}
	//Compare it with existing filenames
	for (uint16_t search_idx =0; search_idx<MAX_FILES; search_idx++)
	{
		if (!strcmp(temp_str,BFFS.files[search_idx].filename))
		{
			*file_ptr_ptr = &(BFFS.files[search_idx]);
		    return OPEN_FILE_SUCCESS;
		}
	}
	return OPEN_FILE_FILE_NOT_FOUND;

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
		*(uint8_t*)(data_ptr+idx) = FRAM[file_ptr->read_ptr];//replace this with read_byte_FRAM function so that this is loosely coupled
		file_ptr->read_ptr++;
	}
	file_ptr->read_ptr = 0;
	return READ_FILE_SUCCESS;
}

//clear file, reset write pointer and write all 0s
//getfsfree bytes
//getfilefree bytes
//loadFS
//ftell
//fseek
//freadat
//fwriteat
//getmax files
//get FS size
//get n files

int main(void)
{
	bffs_st status;

	file_t* myfile1;
	file_t* myfile2;

	uint8_t data_w1_1[3] = {10,20,30};
	uint8_t data_w1_2[3] = {40,50,60};
	uint8_t data_w2  [3] = {70,80,90};
	uint8_t data_r1  [6];
	uint8_t data_r2  [3];

	char myfilename1[10] = "file1.txt";
	char myfilename2[10] = "file2.txt";

	status = mount_fs(USABLE_SIZE);

	status = create_file(myfilename1,10,&myfile1);
	status = create_file(myfilename2,10,&myfile2);

	status = write_file(myfile1,3,data_w1_1);
	status = write_file(myfile1,3,data_w1_2);
	status = write_file(myfile2,3,data_w2);

	status = read_file(myfile1,6,data_r1);
	status = read_file(myfile2,3,data_r2);

	printf("Data Read from File 1: [%d,%d,%d,%d,%d,%d]\n",data_r1[0],data_r1[1],data_r1[2],data_r1[3],data_r1[4],data_r1[5]);
	printf("Data Read File ptr 2: [%d,%d,%d]\n",data_r2[0],data_r2[1],data_r2[2]);
}



