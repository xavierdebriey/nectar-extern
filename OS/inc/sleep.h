/*
 * sleep.h
 *
 *  Created on: 20 Nov 2016
 *      Author: emile
 */

#ifndef SLEEP_H_
#define SLEEP_H_

#define TIMESTEP 5

extern uint32_t counter;

void startSleepTimer(void);
void stopSleepTimer(void);
void miniSpleepMode(void);
void sleepMode(void);
void wakeupMode(void);
void updateTimestamp(void);

#endif /* SLEEP_H_ */
