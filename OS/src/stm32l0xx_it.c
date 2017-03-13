/**
  ******************************************************************************
  * @file    stm32l0xx_it.c
  * @author  Ac6
  * @version V1.0
  * @date    02-Feb-2015
  * @brief   Default Interrupt Service Routines.
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"
#include "stm32l0xx.h"
#ifdef USE_RTOS_SYSTICK
#include <cmsis_os.h>
#endif
#include "stm32l0xx_it.h"
#include "init.h"

extern UART_HandleTypeDef uart;
extern TIM_HandleTypeDef tim_handle;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            	  	    Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles SysTick Handler, but only if no RTOS defines it.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
#ifdef USE_RTOS_SYSTICK
	osSystickHandler();
#endif
}

void USART1_IRQHandler(void)
{
  HAL_UART_IRQHandler(&uart);
}

void TIM6_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&tim_handle);
}

void SPIx_IRQHandler(void)
{
  HAL_SPI_IRQHandler(&SpiHandle);
}

void TIMx_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&TimHandle2);
}
