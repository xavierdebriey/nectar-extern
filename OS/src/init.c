/*
 * Init.c
 *
 * Fonction d'init
 *
 * Par Jean Sébastien P.R.
 */

#include "take_measures.h"
#include "stm32l0xx_hal.h"
#include "init.h"
#include "sleep.h"
#include "config.h"
#include "simulation.h"
#include "communication.h"

TIM_HandleTypeDef          sleepTimerHandle;
TIM_HandleTypeDef          soundSampleTimerHandle;
ADC_HandleTypeDef          soundADCHandle;
SPI_HandleTypeDef          balanceSpiHandle;

uint32_t sampleCounter = 0;
uint32_t counter_time = 0;
uint32_t checker = 20000;

/*
Sleep timer
*/

void initSleepTimer() {
  uint32_t prescaler = (uint32_t) ((HAL_RCC_GetSysClockFreq() / 500) - 1);

  sleepTimerHandle.Instance = TIM6;

  sleepTimerHandle.Init.Period = (500 * TIMESTEP) - 1;
  sleepTimerHandle.Init.Prescaler = prescaler;
  sleepTimerHandle.Init.ClockDivision = 0;
  sleepTimerHandle.Init.CounterMode = TIM_COUNTERMODE_UP;

  HAL_TIM_Base_Init(&sleepTimerHandle);
}

/*
TIM related functions
*/

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
	if(htim == &soundSampleTimerHandle)
	{
		TIMTRAITEMENT_CLK_ENABLE();
		HAL_NVIC_SetPriority(TIMTRAITEMENT_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(TIMTRAITEMENT_IRQn);
	}
	if(htim == &sleepTimerHandle)
	{
  	__HAL_RCC_TIM6_CLK_ENABLE();
  	HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 1, 0);
  	HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &sleepTimerHandle)
	{
		if (counter_time < measure_time - 1) {
			counter_time++;
		}
		else {
			measure_flag = 1;
			counter_time = 0;
		}
	}

	if(htim == &soundSampleTimerHandle)
	{
		if(HAL_ADC_Start(&soundADCHandle) != HAL_OK)
		{
			errorHandler();
		}
		if(HAL_ADC_PollForConversion(&soundADCHandle, 200000) != HAL_OK)
		{
			errorHandler();
		}
		uwADCxConvertedValue = HAL_ADC_GetValue(&soundADCHandle);

		if(HAL_ADC_Stop(&soundADCHandle) != HAL_OK)
		{
			errorHandler();
		}

		adcWait = 1;

		if(sampleCounter >= NBSAMPLES - 1)
		{
		  if(HAL_TIM_Base_Stop_IT(&soundSampleTimerHandle) != HAL_OK)
		  {
			  /* Starting Error */
			  errorHandler();
		  }
      sampleCounter = 0;
		}
    else {
      sampleCounter++;
    }
	}
}

/*
ADC related functions
*/

void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
  GPIO_InitTypeDef                 internGPIOInitStruct;

  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO clock ****************************************/
  __HAL_RCC_GPIOA_CLK_ENABLE();
  /* ADC1 Periph clock enable */
  __HAL_RCC_ADC1_CLK_ENABLE();

  /*##- 2- Configure peripheral GPIO #########################################*/
  /* ADC3 Channel8 GPIO pin configuration */
  internGPIOInitStruct.Pin = GPIO_PIN_0;
  internGPIOInitStruct.Mode = GPIO_MODE_ANALOG;
  internGPIOInitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &internGPIOInitStruct);
}

/*
I2C related functions
*/

void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
  GPIO_InitTypeDef  internGPIOInitStruct;

  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  I2Cx_SCL_GPIO_CLK_ENABLE();
  I2Cx_SDA_GPIO_CLK_ENABLE();
  /* Enable I2C1 clock */
  I2Cx_CLK_ENABLE();

  /*##-2- Configure peripheral GPIO ##########################################*/
  /* I2C TX GPIO pin configuration  */
  internGPIOInitStruct.Pin       = I2Cx_SCL_PIN;
  internGPIOInitStruct.Mode      = GPIO_MODE_AF_OD;
  internGPIOInitStruct.Pull      = GPIO_PULLUP;
  internGPIOInitStruct.Speed     = GPIO_SPEED_FREQ_HIGH  ;
  internGPIOInitStruct.Alternate = I2Cx_SCL_AF;

  HAL_GPIO_Init(I2Cx_SCL_GPIO_PORT, &internGPIOInitStruct);

  /* I2C RX GPIO pin configuration  */
  internGPIOInitStruct.Pin = I2Cx_SDA_PIN;
  internGPIOInitStruct.Alternate = I2Cx_SDA_AF;

  HAL_GPIO_Init(I2Cx_SDA_GPIO_PORT, &internGPIOInitStruct);

  /*##-3- Configure the NVIC for I2C #########################################*/
  /* NVIC for I2C1 */
  HAL_NVIC_SetPriority(I2Cx_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(I2Cx_IRQn);
}

/**
  * @brief I2C MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO, DMA and NVIC configuration to their default state
  * @param hi2c: I2C handle pointer
  * @retval None
  */
void HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c)
{
  /*##-1- Reset peripherals ##################################################*/
  I2Cx_FORCE_RESET();
  I2Cx_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks ################################*/
  /* Configure I2C Tx as alternate function  */
  HAL_GPIO_DeInit(I2Cx_SCL_GPIO_PORT, I2Cx_SCL_PIN);
  /* Configure I2C Rx as alternate function  */
  HAL_GPIO_DeInit(I2Cx_SDA_GPIO_PORT, I2Cx_SDA_PIN);

  /*##-3- Disable the NVIC for I2C ###########################################*/
  HAL_NVIC_DisableIRQ(I2Cx_IRQn);
}

/*
BalanceSPI related functions
*/
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
  GPIO_InitTypeDef  internGPIOInitStruct;

  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  SPIx_SCK_GPIO_CLK_ENABLE();
  SPIx_MISO_GPIO_CLK_ENABLE();
  SPIx_MOSI_GPIO_CLK_ENABLE();
  /* Enable SPI clock */
  SPIx_CLK_ENABLE();

  /*##-2- Configure peripheral GPIO ##########################################*/
  /* SPI SCK GPIO pin configuration  */
  internGPIOInitStruct.Pin       = SPIx_SCK_PIN;
  internGPIOInitStruct.Mode      = GPIO_MODE_AF_PP;
  internGPIOInitStruct.Pull      = GPIO_NOPULL;
  internGPIOInitStruct.Speed     = GPIO_SPEED_FREQ_HIGH  ;
  internGPIOInitStruct.Alternate = SPIx_SCK_AF;

  HAL_GPIO_Init(SPIx_SCK_GPIO_PORT, &internGPIOInitStruct);

  /* SPI MISO GPIO pin configuration  */
  internGPIOInitStruct.Pin = SPIx_MISO_PIN;
  internGPIOInitStruct.Alternate = SPIx_MISO_AF;

  HAL_GPIO_Init(SPIx_MISO_GPIO_PORT, &internGPIOInitStruct);

  /* SPI MOSI GPIO pin configuration  */
  internGPIOInitStruct.Pin = SPIx_MOSI_PIN;
  internGPIOInitStruct.Alternate = SPIx_MOSI_AF;

  HAL_GPIO_Init(SPIx_MOSI_GPIO_PORT, &internGPIOInitStruct);

  /*##-3- Configure the NVIC for SPI #########################################*/
  /* NVIC for SPI */
  HAL_NVIC_SetPriority(SPIx_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(SPIx_IRQn);
}

void Spi_Pin_Init(void)
{
	//Pin 007
  GPIO_InitTypeDef   internGPIOInitStructure;

  /* Enable GPIOB clock */
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* Configure PB7 pin as input floating */
  internGPIOInitStructure.Mode = GPIO_MODE_INPUT;
  internGPIOInitStructure.Pull = GPIO_NOPULL;
  internGPIOInitStructure.Pin = GPIO_PIN_7;
  internGPIOInitStructure.Speed = GPIO_SPEED_FREQ_HIGH  ;
  HAL_GPIO_Init(GPIOB, &internGPIOInitStructure);
}
