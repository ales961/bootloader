/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include <inttypes.h>
/* USER CODE END Includes */

extern UART_HandleTypeDef huart6;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_USART6_UART_Init(void);

/* USER CODE BEGIN Prototypes */

uint16_t uartReceive(uint8_t *buffer, uint16_t size);
uint16_t uartTransmit(const uint8_t *msg, uint16_t msgSize);
uint8_t uartHasNext();
void uartEnableInterruption();
void uartDisableInterruption();
void uartInit();
void uartPooling();
uint8_t uartIsInterruptionEnabled();
void sendRxBuffer(); //debug
uint16_t rxBufferGetSize();
uint16_t rxBufToFlashBuf(uint8_t* flashBuf, uint16_t rxBufSize);
uint8_t isRxStarted();
uint8_t rxBufferPop();
void debug();

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

