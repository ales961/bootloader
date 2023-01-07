#include "uart.h"

#include "usart.h"

uint8_t usartHardwareTransmitBuffer;
uint8_t usartHardwareReceiveBuffer;

uint8_t uartHardwareTransmitPolling(const uint8_t data, const uint16_t timeOut) {
    return HAL_UART_Transmit(&huart6, &data, 1, timeOut) == HAL_OK;
}

void uartHardwareTransmitInterrupt(uint8_t buffer) {
	usartHardwareTransmitBuffer = buffer;
	HAL_NVIC_EnableIRQ(USART6_IRQn);
	HAL_UART_Transmit_IT(&huart6, &usartHardwareTransmitBuffer, 1);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart == &huart6) {
		uartTransmitIntCallback();
	}
}


uint8_t uartHardwareReceivePolling(uint8_t * const data, const uint16_t timeOut) {
    return HAL_UART_Receive(&huart6, data, 1, timeOut) == HAL_OK;
}

void uartHardwareReceiveInterrupt() {
	HAL_NVIC_EnableIRQ(USART6_IRQn);
	HAL_UART_Receive_IT(&huart6, &usartHardwareReceiveBuffer, 1);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart == &huart6) {
    	uartReceiveIntCallback(usartHardwareReceiveBuffer);
    }
}

void uartHardwareDisableInterrupts() {
    HAL_UART_Abort(&huart6);
    HAL_NVIC_DisableIRQ(USART6_IRQn);
}



