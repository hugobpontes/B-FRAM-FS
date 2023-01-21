/*
 ============================================================================
 Name        : B-FRAM-FileSystem.c
 Author      : hugobpontes
 Version     :
 Copyright   : Your copyright notice
 Description : Basic structure of future file system applications
 ============================================================================
 */
#define MAX_FILES	20
#define FRAM_SIZE 8192
#define MAX_FILENAME_SIZE 10

#define FILE_SIZE 8+(MAX_FILENAME_SIZE)

#define FS_SIZE ((FILE_SIZE)*(MAX_FILES))+8
#define FS_OFFSET FS_SIZE

#define USABLE_SIZE (FRAM_SIZE) - (FS_SIZE)

#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef enum
{
    FS_MOUNT_OPTION_RESET,
	FS_MOUNT_OPTION_LOAD,
	FS_MOUNT_INVALID_FS,
}
	bffs_mount_option;

typedef enum
{
	READ_FILE_RESET_READ_PTR,
	READ_FILE_RESET_DONT_READ_PTR,
}
	bffs_read_file_option;
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
  uint16_t end_ptr;
  uint16_t start_ptr;

} file_system_t;


FRAM_t FRAM;
file_system_t BFFS;

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

/* File System functions */
bffs_st save_fs()
{
	write_FRAM(0,FS_SIZE,&BFFS);
	return SAVE_FS_SUCCESS;
}

bffs_st load_fs()
{
	read_FRAM(0,FS_SIZE,&BFFS);

	//try to look for faulty conditions to validat the fs that is being loaded
	if (BFFS.end_ptr>FRAM_SIZE)
	{
		return MOUNT_FS_INVALID_FS;
	}
	if (BFFS.write_ptr>BFFS.end_ptr)
	{
		return MOUNT_FS_INVALID_FS;
	}
	if (BFFS.file_idx > MAX_FILES)
	{
		return MOUNT_FS_INVALID_FS;
	}
	if (BFFS.start_ptr>BFFS.write_ptr)
	{
		return MOUNT_FS_INVALID_FS;
	}
	return MOUNT_FS_SUCCESS;

}

bffs_st reset_fs(uint16_t fs_size)
{
	if (fs_size > USABLE_SIZE)
	{
		return MOUNT_FS_NO_MEMORY;
	}
	BFFS.file_idx = 0;
	BFFS.start_ptr = FS_OFFSET;
	BFFS.write_ptr = FS_OFFSET;
	BFFS.end_ptr = BFFS.start_ptr+fs_size;
	save_fs();

	return MOUNT_FS_SUCCESS;
}


bffs_st mount_fs(uint16_t fs_size, bffs_mount_option option)
{
	bffs_st status;
	switch (option)
	{
	case FS_MOUNT_OPTION_LOAD:
		status = load_fs();
		break;
	case FS_MOUNT_OPTION_RESET:
		status = reset_fs(fs_size);
		break;
	}
	return status;

}

bffs_st create_file(char* filename, uint16_t file_size, file_t** file_ptr_ptr)
{
	if (file_ptr_ptr == NULL)
	{
		return CREATE_FILE_INVALID_FILE_PTR;
	}
	//Check for available file slots
	if (BFFS.file_idx >= MAX_FILES)
	{
		return CREATE_FILE_NO_FILE_SLOTS;
	}
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
	if ((file_size + BFFS.write_ptr) > BFFS.end_ptr)
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

	save_fs();
	return CREATE_FILE_SUCCESS;
}

bffs_st open_file(char* filename,file_t** file_ptr_ptr)
{
	if (file_ptr_ptr == NULL)
	{
		return OPEN_FILE_INVALID_FILE_PTR;
	}
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
			(*file_ptr_ptr)->read_ptr = (*file_ptr_ptr)->start_ptr; //reset read so any loaded read ptrs are reset
		    return OPEN_FILE_SUCCESS;
		}
	}
	return OPEN_FILE_FILE_NOT_FOUND;

}


uint16_t write_file(file_t* file_ptr, uint16_t data_length, void* data_ptr)
{
	uint16_t status;
	if (file_ptr == NULL)
	{
		return WRITE_FILE_INVALID_FILE_PTR;
	}
	if (data_ptr == NULL)
	{
		return WRITE_FILE_INVALID_DATA_PTR;
	}
	if (data_length == 0)
	{
		return WRITE_FILE_BAD_LENGTH;
	}
	uint16_t write_end_ptr = file_ptr->write_ptr+data_length;
	if (write_end_ptr > file_ptr->end_ptr)
	{
		status = WRITE_FILE_OVERFLOW;
		return status;
	}
	write_FRAM(file_ptr->write_ptr,data_length,data_ptr);
	file_ptr->write_ptr+=data_length;

	save_fs();
	return WRITE_FILE_SUCCESS;

}

bffs_st read_file(file_t* file_ptr, uint16_t data_length, void* data_ptr, bffs_read_file_option option)
{
	if (file_ptr == NULL)
	{
		return READ_FILE_INVALID_FILE_PTR;
	}
	if (data_ptr == NULL)
	{
		return READ_FILE_INVALID_DATA_PTR;
	}
	if (data_length == 0)
	{
		return READ_FILE_BAD_LENGTH;
	}
	uint16_t read_end_ptr = file_ptr->read_ptr+data_length;
	if (read_end_ptr > file_ptr->end_ptr)
	{
		return READ_FILE_OVERFLOW;
	}
	read_FRAM(file_ptr->read_ptr,data_length,data_ptr);
	if (option == READ_FILE_RESET_READ_PTR)
		file_ptr->read_ptr = file_ptr->start_ptr;
	return READ_FILE_SUCCESS;
}

bffs_st clear_file(file_t* file_ptr)
{
	uint8_t zero = 0;

	if (file_ptr == NULL)
	{
		return CLEAR_FILE_INVALID_FILE_PTR;
	}
	for (uint32_t idx = 0; idx<(file_ptr->end_ptr-file_ptr->start_ptr);idx++)
	{
		write_FRAM(file_ptr->start_ptr+idx,1,&zero);
	}

	file_ptr->read_ptr = file_ptr->start_ptr;
	file_ptr->write_ptr = file_ptr->start_ptr;

	save_fs();

	return CLEAR_FILE_SUCCESS;

}

bffs_st seek_file(file_t* file_ptr, uint16_t byte)
{
	if (file_ptr == NULL)
	{
		return SEEK_FILE_INVALID_FILE_PTR;
	}
	if (file_ptr->start_ptr+byte>file_ptr->end_ptr)
	{
		return SEEK_FILE_OVERFLOW;
	}
	file_ptr->read_ptr = file_ptr->start_ptr+byte;
	return SEEK_FILE_SUCCESS;
}

uint16_t tell_file(file_t* file_ptr)
{
	return file_ptr->read_ptr-file_ptr->start_ptr;
}

uint16_t get_fs_free_bytes(void)
{
	return BFFS.end_ptr-BFFS.write_ptr;
}
uint16_t get_fs_size(void)
{
	return BFFS.end_ptr-BFFS.start_ptr;
}
uint16_t get_fs_free_file_slots(void)
{
	return MAX_FILES-BFFS.file_idx;
}
uint16_t get_fs_total_file_slots(void)
{
	return MAX_FILES;
}
uint16_t get_fs_total_files(void)
{
	return BFFS.file_idx;
}

uint16_t get_file_free_bytes(file_t* file_ptr)
{
	return file_ptr->end_ptr-file_ptr->write_ptr;
}
uint16_t get_file_used_bytes(file_t* file_ptr)
{
	return file_ptr->write_ptr-file_ptr->start_ptr;
}
uint16_t get_file_size(file_t* file_ptr)
{
	return file_ptr->end_ptr-file_ptr->start_ptr;
}

//Missing functionality:
//Deleting files (involves managing remaining file slots)
//Untested functionality:
//Loading FS from FRAM

int main(void)
{
	printf("FRAM Size: %d\n",FRAM_SIZE);
	printf("File Size: %d\n",FILE_SIZE);
	printf("Max Files: %d\n",MAX_FILES);
	printf("FS Size =(Max Files*File Size)+8 = FS_OFFSET: %d\n",FS_SIZE);
	printf("Usable Size: FRAM Size - FS Size = %d \n",USABLE_SIZE);
	printf("--------------------------------\n");


	bffs_st status;

	file_t* myfile1;
	file_t* myfile2;
	file_t* myfile2_alt;

	uint8_t data_w1_1[3] = {10,20,30};
	uint8_t data_r;
	uint8_t data_r2[3];


	char myfilename1[10] = "file1.txt";
	char myfilename2[10] = "file2.txt";

	printf("Mounting file system with reset option... \n");
	if ((status = mount_fs(USABLE_SIZE,FS_MOUNT_OPTION_RESET)) != MOUNT_FS_SUCCESS)
		return status;

	/*printf("Mounting file system with load option... \n");
	if ((status = mount_fs(USABLE_SIZE,FS_MOUNT_OPTION_LOAD)) != MOUNT_FS_SUCCESS)
		return status;*/

	printf(" FS Free Bytes: %d \n FS Size: %d\n FS Free File Slots: %d\n FS Total File Slots: %d\n FS Total Files: %d\n",
			get_fs_free_bytes(),
			get_fs_size(),
			get_fs_free_file_slots(),
			get_fs_total_file_slots(),
			get_fs_total_files());

	printf("Creating file 1... \n");

	if((status = create_file(myfilename1,10,&myfile1)) != CREATE_FILE_SUCCESS)
		return status;

	printf("Creating file 2... \n");

	if((status = create_file(myfilename2,10,&myfile2)) != CREATE_FILE_SUCCESS)
		return status;

	printf("Pointlessly opening file after its creation to demonstrate opening feature...\n");
	if((status = open_file(myfilename2,&myfile2_alt)) != OPEN_FILE_SUCCESS)
		return status;

	printf("Bool value to check if both pointers to myfile2 are valid: %d \n",myfile2_alt == myfile2);

	printf(" File 1 Free Bytes: %d \n File Taken Bytes: %d\n Total File Size: %d\n File Read pointer at :%d\n",
			get_file_free_bytes(myfile1),
			get_file_used_bytes(myfile1),
			get_file_size(myfile1),
			tell_file(myfile1));

	printf("Writing [%d,%d,%d] to file 1...\n",data_w1_1[0],data_w1_1[1],data_w1_1[2]);
	if ((status = write_file(myfile1,3,data_w1_1)) != WRITE_FILE_SUCCESS)
		return status;
	printf("Seeking byte 2 of file 1...\n");
	if ((status = seek_file(myfile1,2)) != SEEK_FILE_SUCCESS)
		return status;
	printf(" File 1 Free Bytes: %d \n File Taken Bytes: %d\n Total File Size: %d\n File Read pointer at: %d\n",
			get_file_free_bytes(myfile1),
			get_file_used_bytes(myfile1),
			get_file_size(myfile1),
			tell_file(myfile1));

	if ((status = read_file(myfile1,1,&data_r,READ_FILE_RESET_READ_PTR)) != READ_FILE_SUCCESS)
		return status;
	printf("On file 1: have read 1 byte at sought position, resetting read pointer, data: %d \n",data_r);

	printf("Clearing file 1\n");
	if ((status = clear_file(myfile1)) != CLEAR_FILE_SUCCESS)
		return status;

	if ((status = read_file(myfile1,3,&data_r2,READ_FILE_RESET_READ_PTR)) != READ_FILE_SUCCESS)
		return status;
	printf("On file 1: have 3 bytes at start, resetting read pointer, data: [%d,%d,%d] \n",data_r2[0],data_r2[1],data_r2[2]);

	printf(" FS Free Bytes: %d \n FS Size: %d\n FS Free File Slots: %d\n FS Total File Slots: %d\n FS Total Files: %d\n",
			get_fs_free_bytes(),
			get_fs_size(),
			get_fs_free_file_slots(),
			get_fs_total_file_slots(),
			get_fs_total_files());
}




