/*
 * sleep.c
 *
 *  Created on: 20 Nov 2016
 *      Author: emile
 */

#include "filesystem.h"
#include "simulation.h"
#include "communication.h"
#include "config.h"
#include "sleep.h"

#include "stm32l0xx_hal.h"

#define TIMESTEP 60

TIM_HandleTypeDef tim_handle;
uint32_t inreset = 0;

void initTimer() {
	uint32_t prescaler = (uint32_t) ((HAL_RCC_GetSysClockFreq() / 500) - 1);

	tim_handle.Instance = TIM6;

	tim_handle.Init.Period = (500 * TIMESTEP) - 1;
	tim_handle.Init.Prescaler = prescaler;
	tim_handle.Init.ClockDivision = 0;
	tim_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
	HAL_TIM_Base_Init(&tim_handle);
}

void startTimer() {
	HAL_TIM_Base_Start_IT(&tim_handle);
	measure_flag = 0;
}

void stopTimer() {
	HAL_TIM_Base_DeInit(&tim_handle);
	measure_flag = 0;
}

void sleep_mode() {
	// disable the tick interrupt
	HAL_SuspendTick();
	// entering sleep mode
	HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
}

void wakeup_mode() {
	// enable the tick interrupt
	HAL_ResumeTick();
}

void update_timestamp() {
	timestamp += (TIMESTEP * measure_time);
}