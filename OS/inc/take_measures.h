/*
 * take_mesurement.h
 *
 * Paramètre et macro de take_mesurement.c
 *
 *  Par Jean Sébastien Pelletier-Rioux
 *
 */

#ifndef __TAKEMESUREMENT_H
#define __TAKEMESUREMENT_H

#include "stm32l0xx_hal.h"

#define I2C_ADDRESS				0x27
#define I2C_TIMING_100KHZ       0x10A13E56

//extern uint32_t measure_flag;
extern __IO uint32_t uwADCxConvertedValue;
extern int adcWait;
extern int cnt;
extern int nbPoint;

void take_measures(void);

/* Fonction d'handling d'erreur */
void errorHandler(void);

#endif
