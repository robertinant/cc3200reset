#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>

#if defined(__APPLE__)
#include "ftdi.h"
#endif

#ifdef __linux__
#include <ftdi.h>
#endif

#if defined(_WIN32)
#include <windows.h>
#include <ftd2xx.h>
#endif

#if defined(__APPLE__) || defined(__linux__)
struct ftdi_context ftdic;
int tty_fd;
#else
FT_HANDLE ftdic;
HANDLE tty_fd;
#endif

bool init_ftdi();
void reset(void *ptr);

int main() {
	init_ftdi();
	reset(&ftdic);
}

void reset(void *ptr)
{
#if defined(__APPLE__) || defined(__linux__)
	struct ftdi_context *ftdi;
	ftdi = (struct ftdi_context *) ptr;
#else
	DWORD bytes;
	FT_HANDLE *ftdi;
	ftdi = (FT_HANDLE *)ptr;
#endif
	unsigned char b = 0;

	b &= ~(1 << 6);
	b |= (1 << 0);
	b &= ~(1 << 5);
#if defined(__APPLE__) || defined(__linux__)
	ftdi_write_data(ftdi, &b, 1);
#else
	FT_Write(ftdic, &b, 1, &bytes);
#endif
	usleep(5000);
	b |= (1 << 5);
#if defined(__APPLE__) || defined(__linux__)
	ftdi_write_data(ftdi, &b, 1);
#else
	FT_Write(ftdic, &b, 1, &bytes);
#endif
}

bool init_ftdi()
{
	unsigned char b = 1 << 5;

#if defined(__APPLE__) || defined(__linux__)
	ftdi_init(&ftdic);

	ftdi_set_interface(&ftdic, INTERFACE_A);

	if(ftdi_usb_open(&ftdic, 0x0451 , 0xC32A) < 0) {
		puts("Can't open device");
		return false;
	}

	ftdi_write_data_set_chunksize(&ftdic, 1);
	ftdi_set_bitmode(&ftdic, (1 << 0 | 1 << 5 | 1 << 6), BITMODE_BITBANG);

	b &= ~(1 << 6);
	if(ftdi_write_data(&ftdic, &b, 1) < 0){
		printf("write failed");
		return false;
	}

	usleep(100000);
	b |= (1 << 0);
	if(ftdi_write_data(&ftdic, &b, 1) < 0) {
		printf("write failed");
		return false;
	}
#else //Windows
	DWORD bytes;

	if(FT_OpenEx("cc3200A", FT_OPEN_BY_SERIAL_NUMBER, &ftdic)
	&& FT_OpenEx("cc3101A", FT_OPEN_BY_SERIAL_NUMBER, &ftdic)) {
		puts("Can't open device");
		return false;
	}

	FT_SetBitMode(ftdic, (1 << 0 | 1 << 5 | 1 << 6), FT_BITMODE_ASYNC_BITBANG);

	b &= ~(1 << 6);
	if(FT_Write(ftdic, &b, 1, &bytes) < 0) {
		printf("write failed");
		return false;
	}

	usleep(100000);
	b |= (1 << 0);
	if(FT_Write(ftdic, &b, 1, &bytes) < 0) {
		printf("write failed");
		return false;
	}
#endif
	return true;
}

