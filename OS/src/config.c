/*
 * config.c
 *
 *  Created on: 20 Nov 2016
 *      Author: emile
 */

#include "config.h"
#include "sleep.h"
#include "take_measures.h"
#include "iir.h"

#include "stm32l0xx_hal.h"

uint32_t run = 0;

float timestamp;

uint32_t measure_time;
uint32_t sending_time;
uint32_t freqs[3];
float f11, f12, f21, f22, f13, f23;
float sf;
float dcof[4*ORDRE];
int ccof[2*ORDRE+1];
float dc1[2*ORDRE+1];
float cc1[2*ORDRE+1];
float dc2[2*ORDRE+1];
float cc2[2*ORDRE+1];
float dc3[2*ORDRE+1];
float cc3[2*ORDRE+1];


void start_system() {
	run = 1;
	startSleepTimer();
}

void reset_timestamp() {
	timestamp = 0.0;
}

void initConfig() {
	measure_time = 1;
	sending_time = 1;
	freqs[0] = 250;
	freqs[1] = 350;
	freqs[2] = 450;
	load_coefficient(freqs[0], freqs[1], freqs[2]);
}

void change_measure_time(uint32_t new_mt) {
	measure_time = new_mt;
}

void change_sending_time(uint32_t new_st) {
	sending_time = new_st;
}

void change_freqs(uint32_t new_fs[3]) {
	freqs[0] = new_fs[0];
	freqs[1] = new_fs[1];
	freqs[2] = new_fs[2];
	load_coefficient(freqs[0], freqs[1], freqs[2]);
}

void load_coefficient(uint32_t frequence1, uint32_t frequence2, uint32_t frequence3)
{
    for(int i = 0; i < (2*ORDRE); i++)
    {
    	dc1[i] = 0;
    	cc1[i] = 0;
    	dc2[i] = 0;
    	cc2[i] = 0;
    	dc3[i] = 0;
    	cc3[i] = 0;
    }

    f11 = ((float)frequence1-50)/1000;
    f21 = ((float)frequence1+50)/1000;
    f12 = ((float)frequence2-50)/1000;
    f22 = ((float)frequence2+50)/1000;
    f13 = ((float)frequence3-50)/1000;
    f23 = ((float)frequence3+50)/1000;

    for(int i = 0; i < 4*ORDRE; i++)
    {
    	dcof[i] = 0;
    }

    /* calculate the d coefficients */
    dcof_bwbp( ORDRE, f11, f21, dcof);
    /* calculate the c coefficients */
    ccof_bwbp( ORDRE, ccof);
    /* scaling factor for the c coefficients */
    sf = sf_bwbp( ORDRE, f11, f21 );
    for(int i = 0; i <= 2*ORDRE; i++)
	{
	cc1[i] = ccof[i]*sf;
	}
    /* Output the d coefficients */
    for(int i = 0; i <= 2*ORDRE; i++ )
    {
    	dc1[i] = dcof[i];
    }

    for(int i = 0; i < 4*ORDRE; i++)
    {
    	dcof[i] = 0;
    }

    /* calculate the d coefficients */
    dcof_bwbp( ORDRE, f12, f22, dcof);
    /* calculate the c coefficients */
    ccof_bwbp( ORDRE, ccof);
    /* scaling factor for the c coefficients */
    sf = sf_bwbp( ORDRE, f12, f22 );
    for(int i = 0; i <= 2*ORDRE; i++)
    {
    	cc2[i] = (float)ccof[i]*(float)sf;
    }
    /* Output the d coefficients */
    for(int i = 0; i <= 2*ORDRE; i++ )
    {
    	dc2[i] = (float)dcof[i];
    }

    for(int i = 0; i < 4*ORDRE; i++)
    {
    	dcof[i] = 0;
    }

    /* calculate the d coefficients */
    dcof_bwbp( ORDRE, f13, f23, dcof);
    /* calculate the c coefficients */
    ccof_bwbp( ORDRE, ccof);
    /* scaling factor for the c coefficients */
    sf = sf_bwbp( ORDRE, f13, f23 );
    for(int i = 0; i <= 2*ORDRE; i++)
    {
    	cc3[i] = (float)ccof[i]*(float)sf;
    }
    /* Output the d coefficients */
    for(int i = 0; i <= 2*ORDRE; i++ )
    {
    	dc3[i] = (float)dcof[i];
    }
}
