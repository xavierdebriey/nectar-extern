/*
 * command.h
 *
 *  Created on: 12 Nov 2016
 *      Author: emile
 */

#ifndef COMMAND_H_
#define COMMAND_H_

#define ARGUMENTS_SIZE 4
#define NB_COMMANDS 10

#include "stm32l0xx.h"

#define START 0x00
#define TOGGLERED 0x01
#define TOGGLEGREEN 0x02
#define CHANGEMEASURE 0x03
#define CHANGESENDING 0x04
#define CHANGEFREQS 0x05
#define GETCONFIG 0x06
#define STOP 0x07
#define RSTTIMESTAMP 0x08

typedef struct {
	uint8_t header;
	uint8_t arguments[ARGUMENTS_SIZE];
} Command;

void executeCommand();
uint32_t addCommand2Queue(Command new_command);
Command* getActualCommand();
uint32_t getNumberOfCommands();
void resetCommandQueue();

#endif /* COMMAND_H_ */
