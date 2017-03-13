/*
 * config.h
 *
 *  Created on: 20 Nov 2016
 *      Author: emile
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "stm32l0xx.h"

#define ORDRE 6

extern uint32_t run;

extern float timestamp;

extern uint32_t measure_time;
extern uint32_t sending_time;
extern uint32_t freqs[3];

extern float dc1[2*ORDRE+1];
extern float cc1[2*ORDRE+1];
extern float dc2[2*ORDRE+1];
extern float cc2[2*ORDRE+1];
extern float dc3[2*ORDRE+1];
extern float cc3[2*ORDRE+1];

#define MEASURE_LOW 1
#define MEASURE_HIGH 100

#define SENDING_LOW 1
#define SENDING_HIGH 100

#define FREQ_LOW 1
#define FREQ_HIGH 500

void start_system();
void reset_timestamp();

void load_config(void);
void change_measure_time(uint32_t new_mt);
void change_sending_time(uint32_t new_st);
void change_freqs(uint32_t new_fs[3]);
void load_coefficient(uint32_t frequence1, uint32_t frequence2, uint32_t frequence3);

#endif /* CONFIG_H_ */
