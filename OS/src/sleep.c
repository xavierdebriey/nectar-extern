/*
 * sleep.c
 *
 *  Created on: 20 Nov 2016
 *      Author: emile
 */

#include "init.h"
#include "take_measures.h"
#include "config.h"
#include "sleep.h"

#include "stm32l0xx_hal.h"

void startSleepTimer() {
  HAL_TIM_Base_Start_IT(&sleepTimerHandle);
  measure_flag = 0;
}

void stopSleepTimer() {
  HAL_TIM_Base_DeInit(&sleepTimerHandle);
  measure_flag = 0;
}

void miniSpleepMode() {
	HAL_SuspendTick();
}

void sleepMode() {
	// disable the tick interrupt
	HAL_SuspendTick();
	// entering sleep mode
	HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
}

void wakeupMode() {
	// enable the tick interrupt
	HAL_ResumeTick();
}

void updateTimestamp() {
	timestamp += (TIMESTEP * measure_time);
}
