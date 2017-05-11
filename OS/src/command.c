/*
 * command.c
 *
 *  Created on: 12 Nov 2016
 *      Author: emile
 */
#include <string.h>

#include "stm32l0538_discovery.h"
#include "command.h"
#include "filesystem.h"
#include "config.h"
#include "sleep.h"
#include "communication.h"

Command commands[NB_COMMANDS];
uint32_t command = 0;

uint32_t check_bounds(uint32_t arg, uint32_t low_bound, uint32_t high_bound) {
	if (arg < low_bound || arg > high_bound) {
		return 0;
	}
	else {
		return 1;
	}
}

void executeCommand() {
	Command* command;
	command = getActualCommand();

	if (command->header == START) {
		resetData();
		reset_timestamp();
		start_system();
		sendACK();
	}
	else if (command->header == TOGGLERED) {
		BSP_LED_Toggle(LED3);
	}
	else if (command->header == TOGGLEGREEN) {
		BSP_LED_Toggle(LED4);
	}
	else if (command->header == CHANGEMEASURE) {
		if (check_bounds(command->arguments[0], MEASURE_LOW, MEASURE_HIGH)) {
			change_measure_time((uint32_t) command->arguments[0]);
		}
	}
	else if (command->header == CHANGESENDING) {
		if (check_bounds(command->arguments[0], SENDING_LOW, SENDING_HIGH)) {
			change_sending_time((uint32_t) command->arguments[0]);
		}
	}
	else if(command->header == CHANGEFREQS) {
		uint8_t error = 0;

		uint32_t freq1;
		uint32_t freq2;
		uint32_t freq3;

		if (check_bounds(command->arguments[0], FREQ_LOW/2, FREQ_HIGH/2)) {
			freq1 = command->arguments[0] * 2;
		}
		else error = 1;
		if (check_bounds(command->arguments[1], FREQ_LOW/2, FREQ_HIGH/2)) {
			freq2 = command->arguments[1] * 2;
		}
		else error = 1;
		if (check_bounds(command->arguments[2], FREQ_LOW/2, FREQ_HIGH/2)) {
			freq3 = command->arguments[2] * 2;
		}
		else error = 1;
		if (!error) {
			uint32_t freqs[3] = {freq1, freq2, freq3};
			change_freqs(freqs);
		}
	}
	else if (command->header == GETCONFIG) {
		sendResponse();
	}
	else if (command->header == STOP) {
		run = 0;
		stopSleepTimer();
	}
	else if (command->header == RSTTIMESTAMP) {
		reset_timestamp();
	}
}

uint32_t addCommand2Queue(Command new_command) {
	if (command != NB_COMMANDS) {
		commands[command] = new_command;
		command++;
		return 1;
	}
	else {
		return 0;
	}
}

Command* getActualCommand() {
	if (command != 0) {
		command--;
		return &commands[command];
	}
	else {
		return NULL;
	}
}

uint32_t getNumberOfCommands() {
	return command;
}

void resetCommandQueue() {
	command = 0;
}
