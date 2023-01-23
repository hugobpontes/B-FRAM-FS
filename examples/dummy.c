#include "B-FRAM-FileSystem.h"
#include "fram_driver.h"

file_system_t BFFS;

int main(void)
{
	printf("FRAM Size: %d\n",FRAM_SIZE);
	printf("File Size: %d\n",FILE_STRCT_SIZE);
	printf("Max Files: %d\n",MAX_FILES);
	printf("FS Size =(Max Files*File Size)+8 = FS_OFFSET: %d\n",FS_STRCT_SIZE);
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
	printf("On file 1: have read 3 bytes at start, resetting read pointer, data: [%d,%d,%d] \n",data_r2[0],data_r2[1],data_r2[2]);

	printf(" FS Free Bytes: %d \n FS Size: %d\n FS Free File Slots: %d\n FS Total File Slots: %d\n FS Total Files: %d\n",
			get_fs_free_bytes(),
			get_fs_size(),
			get_fs_free_file_slots(),
			get_fs_total_file_slots(),
			get_fs_total_files());
}



