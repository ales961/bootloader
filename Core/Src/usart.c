/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
#include <string.h>

#include "buffer.h"
#include "uart.h"

#define TX_BUFFER_SIZE 64
#define RX_BUFFER_SIZE 256

static Buffer *txBuffer;
static Buffer *rxBuffer;
static volatile uint8_t interruptionsEnabled = 0;
static volatile uint8_t transmitIntEnd = 1;
uint8_t rxStarted = 0;

static void hardwareReceive();
static void hardwareReceiveIT();
static void hardwareTransmit();
static void hardwareTransmitIT();
uint8_t isRxStarted();

/* USER CODE END 0 */

UART_HandleTypeDef huart6;

/* USART6 init function */

void MX_USART6_UART_Init(void)
{

  /* USER CODE BEGIN USART6_Init 0 */

  /* USER CODE END USART6_Init 0 */

  /* USER CODE BEGIN USART6_Init 1 */

  /* USER CODE END USART6_Init 1 */
  huart6.Instance = USART6;
  huart6.Init.BaudRate = 115200;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART6_Init 2 */

  /* USER CODE END USART6_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART6)
  {
  /* USER CODE BEGIN USART6_MspInit 0 */

  /* USER CODE END USART6_MspInit 0 */
    /* USART6 clock enable */
    __HAL_RCC_USART6_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    /**USART6 GPIO Configuration
    PC6     ------> USART6_TX
    PC7     ------> USART6_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* USART6 interrupt Init */
    HAL_NVIC_SetPriority(USART6_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART6_IRQn);
  /* USER CODE BEGIN USART6_MspInit 1 */

  /* USER CODE END USART6_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART6)
  {
  /* USER CODE BEGIN USART6_MspDeInit 0 */

  /* USER CODE END USART6_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART6_CLK_DISABLE();

    /**USART6 GPIO Configuration
    PC6     ------> USART6_TX
    PC7     ------> USART6_RX
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_6|GPIO_PIN_7);

    /* USART6 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART6_IRQn);
  /* USER CODE BEGIN USART6_MspDeInit 1 */

  /* USER CODE END USART6_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

uint16_t uartTransmit(const uint8_t * const msg, const uint16_t msgSize) {
    if (msg == NULL)
        return 0;
    const uint16_t transmitted = bufferPushValues(txBuffer, msg, msgSize);
    if (interruptionsEnabled) {
        hardwareTransmitIT();
    }
    return transmitted;
}

uint8_t uartTransmitChar(const uint8_t msg) {
    if (msg == NULL)
        return 0;
    const uint8_t transmitted = bufferPush(txBuffer, msg);
    if (interruptionsEnabled) {
        hardwareTransmitIT();
    }
    return transmitted;
}

uint8_t uartHasNext() {
    return bufferHasValues(rxBuffer);
}

void uartInit() {
    txBuffer = bufferCreate(TX_BUFFER_SIZE);
    rxBuffer = bufferCreate(RX_BUFFER_SIZE);
}

void uartPooling() {
    if (!interruptionsEnabled) {
        hardwareTransmit();
        hardwareReceive();
    }
}

uint16_t uartReceive(uint8_t * const buffer, const uint16_t size) {
    const uint16_t received = bufferPopValues(rxBuffer, buffer, size);
    if (interruptionsEnabled) {
        hardwareReceiveIT();
    }
    return received;
}

void uartEnableInterruption() {
    interruptionsEnabled = 1;
    transmitIntEnd = 1;
    hardwareTransmitIT();
    hardwareReceiveIT();
}

void uartDisableInterruption() {
    interruptionsEnabled = 0;
    uartHardwareDisableInterrupts();
}

uint8_t uartIsInterruptionEnabled() {
    return interruptionsEnabled;
}

static void hardwareReceive() {
    if (bufferGetFreeSize(rxBuffer) > 0) {
        uint8_t data = 0;
        if (uartHardwareReceivePolling(&data, 5)) {
            bufferPush(rxBuffer, data);
        }
    }
    if (!bufferHasValues(rxBuffer))
        bufferClear(rxBuffer);
}

static void hardwareReceiveIT() {
    uartHardwareReceiveInterrupt();
}

static void hardwareTransmitIT() {
    if (!transmitIntEnd)
        return;

    if (bufferHasValues(txBuffer)) {
        transmitIntEnd = 0;
        uint8_t data = bufferPop(txBuffer);
        uartHardwareTransmitInterrupt(data);
    }
}

static void hardwareTransmit() {
    if (bufferHasValues(txBuffer)) {
        const uint8_t byte = bufferPeek(txBuffer);
        if (uartHardwareTransmitPolling(byte, 5))
            bufferPop(txBuffer);
    }
}

extern void uartReceiveIntCallback(uint8_t data) {
    if (bufferGetFreeSize(rxBuffer) > 0) {
        bufferPush(rxBuffer, data);
    }
    if (!isRxStarted()) rxStarted = 1;
    if (interruptionsEnabled)
        hardwareReceiveIT();
}

extern void uartTransmitIntCallback() {
    transmitIntEnd = 1;
    if (interruptionsEnabled)
        hardwareTransmitIT();
}

uint16_t rxBufferGetSize() {
	return bufferGetSize(rxBuffer);
}

uint8_t rxBufferPop() {
	return bufferPop(rxBuffer);
}

uint16_t rxBufToProgBuf(uint8_t* buf) {
	return bufferPopValues(rxBuffer, buf, bufferGetSize(rxBuffer));
}

uint8_t isRxStarted() {
	return rxStarted;
}
/* USER CODE END 1 */
