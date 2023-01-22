#include <B-FRAM-FileSystem.h>

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
		return WRITE_FILE_OVERFLOW;
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


