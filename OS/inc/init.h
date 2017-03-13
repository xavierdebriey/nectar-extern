/*
 * Comment.bat
 */

#ifndef __INIT_H
#define __INIT_H

#include "stm32l0xx_hal.h"
#include "stm32l0538_discovery.h"

TIM_HandleTypeDef    			tim_handle;
TIM_HandleTypeDef    			TimHandle2;
ADC_HandleTypeDef           	AdcHandle;
SPI_HandleTypeDef 				SpiHandle;

void Spi_Pin_Init(void);


// Section Pin

// Pin de SPI 2
#define SPIx_SCK_PIN                     GPIO_PIN_13
#define SPIx_SCK_GPIO_PORT               GPIOB
#define SPIx_SCK_AF                      GPIO_AF0_SPI2
#define SPIx_MISO_PIN                    GPIO_PIN_14
#define SPIx_MISO_GPIO_PORT              GPIOB
#define SPIx_MISO_AF                     GPIO_AF0_SPI2
#define SPIx_MOSI_PIN                    GPIO_PIN_15
#define SPIx_MOSI_GPIO_PORT              GPIOB
#define SPIx_MOSI_AF                     GPIO_AF0_SPI2

// Pin de I2C1 Pins
#define I2Cx_SCL_PIN                    GPIO_PIN_8
#define I2Cx_SCL_GPIO_PORT              GPIOB
#define I2Cx_SCL_AF                     GPIO_AF4_I2C1
#define I2Cx_SDA_PIN                    GPIO_PIN_9
#define I2Cx_SDA_GPIO_PORT              GPIOB
#define I2Cx_SDA_AF                     GPIO_AF4_I2C1




// Section horloge

// Horloge du Timer 2 pour le traitement de signal
#define TIMTRAITEMENT                         TIM2
#define TIMTRAITEMENT_CLK_ENABLE              __HAL_RCC_TIM2_CLK_ENABLE

// Horloge pour SPI2
#define SPIx                             SPI2
#define SPIx_CLK_ENABLE()                __HAL_RCC_SPI2_CLK_ENABLE()
#define SPIx_SCK_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPIx_MISO_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPIx_MOSI_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()

#define SPIx_FORCE_RESET()               __HAL_RCC_SPI2_FORCE_RESET()
#define SPIx_RELEASE_RESET()             __HAL_RCC_SPI2_RELEASE_RESET()

// Horloge pour I2C1
#define I2Cx                              I2C1
#define I2Cx_CLK_ENABLE()                __HAL_RCC_I2C1_CLK_ENABLE()
#define I2Cx_SDA_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define I2Cx_SCL_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()

#define I2Cx_FORCE_RESET()               __HAL_RCC_I2C1_FORCE_RESET()
#define I2Cx_RELEASE_RESET()             __HAL_RCC_I2C1_RELEASE_RESET()




// Section NVIC

// SPI2's NVIC
#define SPIx_IRQn                       	SPI2_IRQn
#define SPIx_IRQHandler                 	SPI2_IRQHandler

// NVIC pour le timer 2
#define TIMTRAITEMENT_IRQn					TIM2_IRQn
#define TIMTRAITEMENT_IRQHandler            TIM2_IRQHandler

// I2C1 NVIC
#define I2Cx_IRQn                    		I2C1_IRQn
#define I2Cx_IRQHandler            			I2C1_IRQHandler

#endif
