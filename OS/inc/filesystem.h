/*
 * filesystem.h
 *
 *  Created on: 28 Oct 2016
 *      Author: emile
 */

#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#define BUFFERSIZE 100

#define DEBUG_MODE

#include "stm32l0xx.h"

typedef enum {
	FS_OK       = 0x00,
	FS_EMPTY    = 0x01,
	FS_FULL		= 0x02
} FileSystem_Status;

typedef struct {
	float timestamp;
	float temperature;
	float humidity;
	float weight;
	float volume;
	float freq[3];
} Data;

FileSystem_Status pushData(Data);
Data* readData();
uint8_t nbData(void);
void resetData(void);

#endif /* FILESYSTEM_H_ */
