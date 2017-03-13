/*
 * sleep.h
 *
 *  Created on: 20 Nov 2016
 *      Author: emile
 */

#ifndef SLEEP_H_
#define SLEEP_H_

extern uint32_t counter;

void initTimer(void);
void startTimer(void);
void stopTimer(void);
void sleep_mode(void);
void wakeup_mode(void);
void update_timestamp(void);

#endif /* SLEEP_H_ */
