#include "sdcard_access.h"

static u8 sdcard_read_buf[MAX_FILE_SIZE] __attribute__ ((aligned(32)));
//u8 sdcard_write_buf[MAX_FILE_SIZE] __attribute__ ((aligned(32)));
uint32_t load_lut_iq[MAX_FILE_SIZE/4];

/************************** Function Prototypes ******************************/
int read_wavefrom_from_sdcard(char *file_name,
							uint32_t file_size,
							uint32_t *load_lut_iq)
{
	int status = 0;
	uint32_t num_of_bytes_read = 0;
	status = read_sdcard(file_name, file_size, sdcard_read_buf, &num_of_bytes_read);

	if (status != XST_SUCCESS) {
		xil_printf("SD Card read failed. \r\n");
		return XST_FAILURE;
	}

	xil_printf("Successfully read SD Card. \r\n");
	printf("num of bytes read from SD Card is %d\n", num_of_bytes_read);

	generate_waveform(sdcard_read_buf, num_of_bytes_read, load_lut_iq);
	return XST_SUCCESS;
}

int read_sdcard(char *file_name,
				uint32_t file_size,
				uint8_t *read_buf,
				uint32_t *num_of_bytes_read)
{
	static FIL fil;		/* File object */
	static FATFS fatfs;

	FRESULT Res;
	UINT NumBytesRead;

	u32 BuffCnt;
	BYTE work[FF_MAX_SS];
	u32 FileSize = file_size;

	/*
	 * To test logical drive 0, Path should be "0:/"
	 * For logical drive 1, Path should be "1:/"
	 */
	TCHAR *Path = "0:/";

	/*
	 * Register volume work area, initialize device
	 */
	Res = f_mount(&fatfs, Path, 0);

	if (Res != FR_OK) {
		return XST_FAILURE;
	}

	/*
	 * Path - Path to logical driver, 0 - FDISK format.
	 * 0 - Cluster size is automatically determined based on Vol size.
	 */
	Res = f_mkfs(Path, FM_FAT32, 0, work, sizeof work);
	if ((Res != FR_OK) && (Res != FR_WRITE_PROTECTED)) {
		return XST_FAILURE;
	}

	/*
	 * Open file with required permissions.
	 * Here - Open existing file with read permissions. .
	 * To open file with write permissions, file system should not
	 * be in Read Only mode.
	 */

	Res = f_open(&fil, file_name, FA_OPEN_EXISTING | FA_READ);
	if ((Res != FR_OK) && (Res != FR_WRITE_PROTECTED)) {
		return XST_FAILURE;
	}

	/*
	 * Pointer to beginning of file .
	 */
	Res = f_lseek(&fil, 0);
	if (Res) {
		return XST_FAILURE;
	}

	/*
	 * Read data from file.
	 */
	Res = f_read(&fil, (void*)read_buf, FileSize,
			&NumBytesRead);
	if (Res) {
		return XST_FAILURE;
	}
	*num_of_bytes_read = (uint32_t)NumBytesRead;

#if 0
	/*
	 * Data verification
	 */
	for(BuffCnt = 0; BuffCnt < FileSize; BuffCnt++){
		printf("read_buf[%d] = %d\n", BuffCnt, read_buf[BuffCnt]);
	}
#endif

	/*
	 * Close file.
	 */
	Res = f_close(&fil);
	if (Res) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

void generate_waveform(uint8_t *read_buf, uint32_t num_of_bytes_read, uint32_t *zero_lut_iq)
{
	if(num_of_bytes_read > MAX_FILE_SIZE)
		return;

	for(int i = 0; i < num_of_bytes_read/4; i++)
	{
		zero_lut_iq[i] =	read_buf[i*4+3] << (3*8) |
							read_buf[i*4+2] << (2*8) |
							read_buf[i*4+1] << (1*8) |
							read_buf[i*4+0] << 0;
	}
}
