/*
 * take_mesurement.c
 *
 * Function witch return the value of each captor individually.
 *
 * Param:
 * 	Entré:
 * 		Void
 * 	Sortie:
 * 		void
 *
 *  By Jean Sébastien Pelletier-Rioux
 *
 */
#include <math.h>

#include "stm32l0xx.h"
#include "init.h"
#include "take_measures.h"
#include "iir.h"
#include "config.h"
#include "filesystem.h"
#include "simulation.h"
#include "communication.h"
#include "sleep.h"


int adcWait = 0;
uint32_t measure_flag = 0;
__IO uint32_t uwADCxConvertedValue = 0;

I2C_HandleTypeDef 				temperatureI2CHandle;
GPIO_InitTypeDef 				balanceGPIOInitStruct;
ADC_ChannelConfTypeDef          sConfig;

void take_measures(void)
{
	updateTimestamp();
	
	Data data;
	data.timestamp = timestamp;

	//Table des Buffers
	uint8_t buffer_I2C[4];

	//variable
	int reading_hum = 0;
	int reading_temp = 0;
	float humidity = 0;
	float temperature = 0;

	int j = 0, k = 0;

	/* Humidity & temperature sensor section
	 The humidity and temperature sensor work with I2C communication. The code create an handle for the
	 I2C witch is then use to initialize the communication with the HAL library. The sensor is read in block mode
	 with the memory read function from the HAL library.
	*/

	temperatureI2CHandle.Instance              = I2Cx;
	temperatureI2CHandle.Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
	temperatureI2CHandle.Init.Timing           = I2C_TIMING_100KHZ;
	temperatureI2CHandle.Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
	temperatureI2CHandle.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
	temperatureI2CHandle.Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
	temperatureI2CHandle.Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;
	temperatureI2CHandle.Init.OwnAddress1      = I2C_ADDRESS;
	temperatureI2CHandle.Init.OwnAddress2      = 0xFE;

	if(HAL_I2C_Init(&temperatureI2CHandle) != HAL_OK)
	{
		/* Initialization Error */
	    errorHandler();
	}

	int failed2connect = 0;
	while(HAL_I2C_Mem_Read(&temperatureI2CHandle, (uint16_t)I2C_ADDRESS<<1, 0, 1, (uint8_t *)buffer_I2C, 4, 1000) != HAL_OK)
	{
		if (HAL_I2C_GetError(&temperatureI2CHandle) != HAL_I2C_ERROR_AF)
		{
			errorHandler();
		}

		if (failed2connect > 5) break;
		failed2connect++;
	}

	reading_hum = 0;
	reading_temp = 0;
	reading_hum = (buffer_I2C[0] << 8) + buffer_I2C[1];
	humidity = reading_hum / 16382.0 * 100.0 - 100;
	reading_temp = (buffer_I2C[2] << 6) + (buffer_I2C[3] >> 2);
	temperature = reading_temp / 16382.0 * 165.0 - 40;
	data.humidity = humidity;
	data.temperature = temperature;

	// balance part
	// data.weigth = measureBalance();
	data.weight = 0.0;

	/*
	 * Micro part
	 * Signal processing
	 */

    float xBuffer[2*ORDRE+1], y1Buffer[2*ORDRE+1], y2Buffer[2*ORDRE+1], y3Buffer[2*ORDRE+1];
    float x1 = 0;
    float x2 = 0;
    float x3 = 0;
    double ind1,ind2,ind3;
    adcWait = 0;
    float signalFiltrer1 = 0.0, signalFiltrer2 = 0.0, signalFiltrer3 = 0.0;

    for(j = 0; j < 2*ORDRE+1; j++)
    {
    	xBuffer[j] = 0;
        y1Buffer[j] = 0;
        y2Buffer[j] = 0;
        y3Buffer[j] = 0;
    }

    initSoundADC();

	/* ### - 2 - Start calibration ############################################ */
	if (HAL_ADCEx_Calibration_Start(&soundADCHandle, ADC_SINGLE_ENDED) != HAL_OK)
	{
	  errorHandler();
	}

	/* ### - 3 - Channel configuration ######################################## */
	/* Select Channel 0 to be converted */
	sConfig.Channel = ADC_CHANNEL_0;
	if (HAL_ADC_ConfigChannel(&soundADCHandle, &sConfig) != HAL_OK)
	{
	  errorHandler();
	}

    initSoundSampleTimer();

    for(j = 0; j < (2*ORDRE+1); j++)
    {
    	xBuffer[j] = 0;
    }

    /*##-2- Start the TIM Base generation in interrupt mode ####################*/
    /* Start Channel1 */
    if(HAL_TIM_Base_Start_IT(&soundSampleTimerHandle) != HAL_OK)
    {
      errorHandler();
    }

	for (j = 0; j < NBSAMPLES; j++)
	{

		for (k = (2*ORDRE); k>0; k--)
		{
			xBuffer[k] = xBuffer[k-1];
			y1Buffer[k] = y1Buffer[k-1];
		}

		while(sampleCounter < j) asm("nop");	

		xBuffer[0]=uwADCxConvertedValue-2048;
		x1 = xBuffer[0]*cc1[0];

		for (k = 1; k < (2*ORDRE+1); k++)
		{
			x1 += (xBuffer[k]*cc1[k]) - (y1Buffer[k]*dc1[k]);
		}

		y1Buffer[0] = x1;

		signalFiltrer1 += fabsf(x1);
	}

	HAL_Delay(10);

	for(j = 0; j < (2*ORDRE+1); j++)
    {
    	xBuffer[j] = 0;
    }

    if(HAL_TIM_Base_Start_IT(&soundSampleTimerHandle) != HAL_OK)
    {
      errorHandler();
    }

	for (j = 0; j < NBSAMPLES; j++)
	{

		for (k = (2*ORDRE); k>0; k--)
		{
			xBuffer[k] = xBuffer[k-1];
			y2Buffer[k] = y2Buffer[k-1];
		}

		while(sampleCounter < j) asm("nop");

		xBuffer[0]=uwADCxConvertedValue-2048;
		x2 = xBuffer[0]*cc2[0];

		for (k = 1; k < (2*ORDRE+1); k++)
		{
			x2 += (xBuffer[k]*cc2[k]) - (y2Buffer[k]*dc2[k]);
		}

		y2Buffer[0] = x2;

		signalFiltrer2 += fabsf(x2);
	}

	HAL_Delay(10);

	for(j = 0; j < (2*ORDRE+1); j++)
    {
    	xBuffer[j] = 0;
    }

    if(HAL_TIM_Base_Start_IT(&soundSampleTimerHandle) != HAL_OK)
    {
      errorHandler();
    }

	for (j = 0; j < NBSAMPLES; j++)
	{

		for (k = (2*ORDRE); k>0; k--)
		{
			xBuffer[k] = xBuffer[k-1];
			y3Buffer[k] = y3Buffer[k-1];
		}

		while(sampleCounter < j) asm("nop");

		xBuffer[0]=uwADCxConvertedValue-2048;
		x3 = xBuffer[0]*cc3[0];

		for (k = 1; k < (2*ORDRE+1); k++)
		{
			x3 += (xBuffer[k]*cc3[k]) - (y3Buffer[k]*dc3[k]);
		}

		y3Buffer[0] = x3;

		signalFiltrer3 += fabsf(x3);
	}

	ind1 = log(signalFiltrer1);
	ind2 = log(signalFiltrer2);
	ind3 = log(signalFiltrer3);

	data.volume = uwADCxConvertedValue;
	data.freq[0] = ind1;
	data.freq[1] = ind2;
    data.freq[2] = ind3;

	pushData(data);
	measure_flag = 0;
}

float measureBalance() {
	int i = 0, j = 0;

	float weight_buffer[20];
	int get_weight;
	float convert_weight;

	uint8_t buffer_SPI_RX[3];
	buffer_SPI_RX[0] = 0;
	buffer_SPI_RX[1] = 0;
	buffer_SPI_RX[2] = 0;

	balanceSpiHandle.Instance               = SPIx;
	balanceSpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	balanceSpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
	balanceSpiHandle.Init.CLKPhase          = SPI_PHASE_1EDGE;
	balanceSpiHandle.Init.CLKPolarity       = SPI_POLARITY_HIGH;
	balanceSpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
	balanceSpiHandle.Init.CRCPolynomial     = 7;
	balanceSpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
	balanceSpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
	balanceSpiHandle.Init.NSS               = SPI_NSS_SOFT;
	balanceSpiHandle.Init.TIMode            = SPI_TIMODE_DISABLE;
	balanceSpiHandle.Init.Mode = SPI_MODE_MASTER;

	if(HAL_SPI_Init(&balanceSpiHandle) != HAL_OK)
	{
	  errorHandler();
	}

	Spi_Pin_Init();

	__HAL_RCC_GPIOB_CLK_ENABLE();

	balanceGPIOInitStruct.Pin = (GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6);
	balanceGPIOInitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	balanceGPIOInitStruct.Pull = GPIO_PULLUP;
	balanceGPIOInitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(GPIOB, &balanceGPIOInitStruct);

	GPIOB->BRR |= GPIO_PIN_3;
	while(i<8)
	{	//set pin PB4 to high (activate the l'ADC)
		GPIOB->BRR |= GPIO_PIN_5;
		GPIOB->BRR |= GPIO_PIN_6;
		while(j<20000){j++;} 		j = 0;

		GPIOB->BSRR |= GPIO_PIN_4;
		while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) != 0){}
		takeBalance(buffer_SPI_RX);

		get_weight = (buffer_SPI_RX[0] << 16) + (buffer_SPI_RX[1] << 8) + buffer_SPI_RX[2];
		convert_weight = get_weight/(pow(2, 24))*5;
		weight_buffer[i] = convert_weight;
		i++;

		GPIOB->BSRR |= GPIO_PIN_5;
		GPIOB->BRR |= GPIO_PIN_6;
		while(j<20000){j++;}		j = 0;

		GPIOB->BSRR |= GPIO_PIN_4;
		while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) != 0){}
		takeBalance(buffer_SPI_RX);

		get_weight = (buffer_SPI_RX[0] << 16) + (buffer_SPI_RX[1] << 8) + buffer_SPI_RX[2];
		convert_weight = get_weight/(pow(2, 24))*5;
		weight_buffer[i] = convert_weight;
		i++;

		GPIOB->BRR |= GPIO_PIN_5;
		GPIOB->BSRR |= GPIO_PIN_6;
		while(j<20000){j++;}		j = 0;

		GPIOB->BSRR |= GPIO_PIN_4;
		while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) != 0){}
		takeBalance(buffer_SPI_RX);

		get_weight = (buffer_SPI_RX[0] << 16) + (buffer_SPI_RX[1] << 8) + buffer_SPI_RX[2];
		convert_weight = get_weight/(pow(2, 24))*5;
		weight_buffer[i] = convert_weight;
		i++;

		GPIOB->BSRR |= GPIO_PIN_5;
		GPIOB->BSRR |= GPIO_PIN_6;
		while(j<20000){j++;}		j = 0;

		GPIOB->BSRR |= GPIO_PIN_4;
		while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) != 0){}
		takeBalance(buffer_SPI_RX);

		get_weight = (buffer_SPI_RX[0] << 16) + (buffer_SPI_RX[1] << 8) + buffer_SPI_RX[2];
		convert_weight = get_weight/(pow(2, 24))*5;
		weight_buffer[i] = convert_weight;
		i++;
	}

	float sum_mesure = 0.0;
	for(i = 0; i<8; i++)
	{
		sum_mesure += weight_buffer[i];
	}

	return sum_mesure/8;
}

/*
 * errorHandler
 * If an error happen, go through an infinite loop instead of doing some shenanigan.
 */

void errorHandler(void)
{
  /* boucle infini */
  sendLog("ERROR HANDLER", 0);
  while(1){}
}

void takeBalance(uint8_t* buffer)
{
	if (HAL_SPI_Receive_IT(&balanceSpiHandle, buffer, 3) != HAL_OK)
	{
	  errorHandler();
	}

	while (HAL_SPI_GetState(&balanceSpiHandle) != HAL_SPI_STATE_READY) {}

	//Close the ADC
	GPIOB->BRR |= GPIO_PIN_4 ;
}

void initSoundADC(void)
{
	/* ### - 1 - Initialize ADC peripheral #################################### */
	/*
	 *  Instance                  = ADC1.
	 *  OversamplingMode          = Disabled
	 *  ClockPrescaler            = PCLK clock with no division.
	 *  LowPowerAutoPowerOff      = Disabled (For this exemple continuous mode is enabled with software start)
	 *  LowPowerFrequencyMode     = Enabled (To be enabled only if ADC clock is lower than 2.8MHz)
	 *  LowPowerAutoWait          = Disabled (New conversion starts only when the previous conversion is completed)
	 *  Resolution                = 12 bit (increased to 16 bit with oversampler)
	 *  SamplingTime              = 7.5 cycles od ADC clock.
	 *  ScanConvMode              = Forward
	 *  DataAlign                 = Right
	 *  ContinuousConvMode        = Enabled
	 *  DiscontinuousConvMode     = Enabled
	 *  ExternalTrigConvEdge      = None (Software start)
	 *  EOCSelection              = End Of Conversion event
	 *  DMAContinuousRequests     = DISABLE
	 */

	soundADCHandle.Instance = ADC1;
	soundADCHandle.Init.OversamplingMode      = DISABLE;
	soundADCHandle.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV4;
	soundADCHandle.Init.LowPowerAutoPowerOff  = DISABLE;
	soundADCHandle.Init.LowPowerFrequencyMode = ENABLE;
	soundADCHandle.Init.LowPowerAutoWait      = DISABLE;
	soundADCHandle.Init.Resolution            = ADC_RESOLUTION_12B;
	soundADCHandle.Init.SamplingTime          = ADC_SAMPLETIME_7CYCLES_5;
	soundADCHandle.Init.ScanConvMode          = ADC_SCAN_DIRECTION_FORWARD;
	soundADCHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
	soundADCHandle.Init.ContinuousConvMode    = ENABLE;
	soundADCHandle.Init.DiscontinuousConvMode = DISABLE;
	soundADCHandle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
	soundADCHandle.Init.EOCSelection          = ADC_EOC_SINGLE_CONV;
	soundADCHandle.Init.DMAContinuousRequests = DISABLE;

	if (HAL_ADC_Init(&soundADCHandle) != HAL_OK)
	{
	  errorHandler();
	}
}

void initSoundSampleTimer(void)
{
	/* Compute the prescaler value to have TIMx counter clock equal to 1 KHz */
    uint16_t uwPrescalerValue = (uint16_t) ((SystemCoreClock / 10000) - 1);
    /*##-1- Configure the TIM peripheral #######################################*/
    /* Set TIMx instance */
    soundSampleTimerHandle.Instance = TIMTRAITEMENT;

    /* Initialize TIMx peripheral as follow:
         + Period = 10000 - 1
         + Prescaler = SystemCoreClock/10000 Note that APB clock = TIMx clock if
                       APB prescaler = 1.
         + ClockDivision = 0
         + Counter direction = Up
    */
    soundSampleTimerHandle.Init.Period = 5 - 1;
    soundSampleTimerHandle.Init.Prescaler = uwPrescalerValue;
    soundSampleTimerHandle.Init.ClockDivision = 0;
    soundSampleTimerHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
    if(HAL_TIM_Base_Init(&soundSampleTimerHandle) != HAL_OK)
    {
      /* Initialization Error */
      errorHandler();
    }
}

