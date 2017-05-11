/*
 * communication.c
 *
 *  Created on: 9 Nov 2016
 *      Author: emile
 */

#include "communication.h"
#include "command.h"
#include "config.h"
#include "filesystem.h"

#include <string.h>

#include "stm32l0538_discovery.h"

UART_HandleTypeDef uart;
uint32_t uart_ready = 0;
Command data;

/*
 * private prototypes
 */

Communication_Status init_uart();
Communication_Status init_xbee();

/*
 * Functions definitions
 */

Communication_Status initCommSystem() {
	if (init_uart() != COMM_OK) return COMM_ERROR;
//	if (init_xbee() != COMM_OK) return COMM_ERROR; // for a future version ..
	recvData(); // start to recv commands
	return COMM_OK;
}

Communication_Status init_uart() {
	// Initialize UART
	uart.Instance = USART1;
	//
	uart.Init.BaudRate = 9600;
	uart.Init.WordLength = UART_WORDLENGTH_8B;
	uart.Init.StopBits = UART_STOPBITS_1;
	uart.Init.Parity = UART_PARITY_NONE;
	uart.Init.Mode = UART_MODE_TX_RX;
	uart.Init.HwFlowCtl = UART_HWCONTROL_NONE ;
	//
	if (HAL_UART_Init(&uart) != HAL_OK) return COMM_ERROR;
	else {
		uart_ready = 1;
		return COMM_OK;
	}
}

Communication_Status init_xbee() {
	char* status = "ATMY\r";
	char* quit = "ATCN\r";
	//char ack[] = "OK\r";
	char answer[64];

	HAL_UART_Transmit(&uart, (uint8_t*) "+++", 3, 5000);
	HAL_UART_Receive(&uart, (uint8_t*) answer, 64, 5000);

	HAL_UART_Transmit(&uart, (uint8_t*) status, strlen(status), 5000);
	HAL_UART_Receive(&uart, (uint8_t*) answer, 64, 5000);

	HAL_UART_Transmit(&uart, (uint8_t*) quit, strlen(quit), 5000);
	HAL_UART_Receive(&uart, (uint8_t*) answer, 64, 5000);

//	for (int i = 0; i < 100; i++) {
//		HAL_UART_Transmit(&uart, (uint8_t*) "HELLO WORLD!", strlen("HELLO WORLD!"), 1000);
//		HAL_Delay(2000);
//	}

	return COMM_OK;
}

Communication_Status recvData() {
	// Check if the uart comm has been initiated
	if (uart_ready != 1) {
		return COMM_ERROR;
	}
	
	// Recv the data
	if (HAL_UART_Receive_IT(&uart, (uint8_t*) &data, sizeof(Command)) != HAL_OK) return COMM_ERROR;
	else return COMM_OK;
}

Communication_Status sendData(uint8_t *pData, uint16_t size) {
	// Check if the uart comm has been initiated
	if (uart_ready != 1) {
		return COMM_ERROR;
	}

	// Send the data
	if (HAL_UART_Transmit(&uart, pData, size, 1000) != HAL_OK) return COMM_ERROR;
	else return COMM_OK;
}

void sendAllData() {
	Data* data;
	uint8_t nb_data = nbData();
	sendData((uint8_t*) "dat", 3);
	sendData(&nb_data, 1);
	data = readData();
	while(data != NULL) {
		sendData((uint8_t*) data, sizeof(Data));
		data = readData();
	}
	sendData((uint8_t*) "\n\n", 2);
}

void sendResponse() {
	sendData((uint8_t*) "rep", 3);
	sendData((uint8_t*) &(measure_time), 4);
	sendData((uint8_t*) &(sending_time), 4);
	sendData((uint8_t*) &(freqs), 12);
	sendData((uint8_t*) "\n\n", 2);
}

void sendLog(char* text2log, int arg) {
	char str2send[100] = {0};
	sprintf(str2send, "%s %d", text2log, arg);
	sendData((uint8_t*) "log", 3);
	sendData((uint8_t*) str2send, strlen(str2send));
	sendData((uint8_t*) "\n\n", 2);
}

void sendACK() {
	sendData((uint8_t*) "ack", 3);
	sendData((uint8_t*) "\n\n", 2);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *uart_handle) {

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *uart_handle) {
	addCommand2Queue(data);
	recvData();
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *uart_handle) {
	uint32_t error = HAL_UART_GetError(uart_handle);
	if (error == HAL_USART_ERROR_DMA) {

	}
	else if (error == HAL_USART_ERROR_PE) {

	}
	else if (error == HAL_USART_ERROR_NE) {

	}
	else if (error == HAL_USART_ERROR_FE) {

	}
	else if (error == HAL_USART_ERROR_ORE) {

	}
}

void HAL_UART_MspInit(UART_HandleTypeDef *uart_handle) {
	GPIO_InitTypeDef gpio_handle;

	__HAL_RCC_USART1_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	// init Tx and Rx
	gpio_handle.Pin = GPIO_PIN_9 | GPIO_PIN_10;
	gpio_handle.Mode = GPIO_MODE_AF_PP;
	gpio_handle.Pull  = GPIO_NOPULL;
	gpio_handle.Speed = GPIO_SPEED_FAST;
	gpio_handle.Alternate = GPIO_AF4_USART1;
	//
	HAL_GPIO_Init(GPIOA, &gpio_handle);

	// activate interruption
	HAL_NVIC_SetPriority(USART1_IRQn, 0, 1);
	HAL_NVIC_EnableIRQ(USART1_IRQn);
}





