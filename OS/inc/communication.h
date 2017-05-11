/*
 * communication.h
 *
 *  Created on: 9 Nov 2016
 *      Author: emile
 */

#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#include "command.h"

typedef enum {
	COMM_OK       = 0x00,
	COMM_ERROR    = 0x01,
} Communication_Status;

Communication_Status initCommSystem();
Communication_Status recvData();
Communication_Status sendData(uint8_t *pData, uint16_t size);
void sendAllData(void);
void sendResponse();
void sendLog(char* text2log, int arg);
void sendACK();

#endif /* COMMUNICATION_H_ */
