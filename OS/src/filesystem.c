/*
 * filesystem.c
 *
 *  Created on: 28 Oct 2016
 *      Author: emile
 */

#include "filesystem.h"

#include "stm32l0xx.h"
#include "stm32l0538_discovery.h"
#include "stm32l0538_discovery_epd.h"

Data buffer[BUFFERSIZE];
uint8_t ptr = 0;

FileSystem_Status pushData(Data src) {
	if (ptr != BUFFERSIZE) {
		buffer[ptr] = src;
		ptr++;
		return FS_OK;
	}
	else {
		return FS_FULL;
	}
}

Data* readData() {
	if (ptr != 0) {
		ptr--;
		return &buffer[ptr];
	}
	else {
		return NULL;
	}
}

uint8_t nbData() {
	return ptr;
}

void resetData() {
	ptr = 0;
}
