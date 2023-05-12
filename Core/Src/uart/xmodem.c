#include <uart/xmodem.h>
#include "usart.h"
#include "hex_parser.h"
#include "tim.h"


/* Global variables. */
uint8_t xmodemPacketNumber = 1u;         /**< Packet number counter. */
uint8_t xmodemBuf[256];
uint16_t xmodemBufSize = 0;
uint8_t remainingBuf[128];
uint16_t remainingSize = 0;
uint8_t timerWorking = 0;


/* Local functions. */
static uint16_t xmodemCalcCrc(uint8_t *data, uint16_t length);
static xmodem_status xmodem_handle_packet(uint8_t size);
static xmodem_status xmodem_error_handler(uint8_t *error_number, uint8_t max_error_number);
void add_remaining_data_to_current_buf(uint8_t* receivedData);
void cut_buf();

/**
 * @brief   This function is the base of the Xmodem protocol.
 *          When we receive a header from UART, it decides what action it shall take.
 * @param   void
 * @return  uint8_t
 */
uint8_t xmodemReceive() {
    volatile xmodem_status status = X_OK;
    uint8_t error_number = 0;
    uint8_t header = 0x00u;
    xmodemPacketNumber = 1u;
    HAL_TIM_Base_Start_IT(&htim6); //send "C" every 10 sec
    timerWorking = 1;
    /* Loop until there isn't any error or done. */
    while (status == X_OK) {
  	    WRITE_REG(IWDG->KR, 0x0000AAAAU);
	    xmodemBufSize = uartReceive(xmodemBuf, 1);
	    if (xmodemBufSize != 1) continue;
	    if (timerWorking) {
	    	HAL_TIM_Base_Stop_IT(&htim6); //stop sending "C"
	    	timerWorking = 0;
	    }
    	HAL_Delay(100);
        header = xmodemBuf[0];

        xmodem_status packet_status = X_ERROR;
        /* The header can be: SOH, STX, EOT and CAN. */
        switch (header) {
            case X_SOH:
                packet_status = xmodem_handle_packet(header); //If the handling was successful, then send an ACK
                if (packet_status == X_OK) {
                	uartTransmitChar(X_ACK);
                } else if (packet_status == X_WRONG_FIRMWARE) {
                	xmodem_error_handler(&error_number, 0);
                	status = X_WRONG_FIRMWARE;
                } else {
                	status = xmodem_error_handler(&error_number, X_MAX_ERRORS); //Error while processing the packet, either send a NAK or do graceful abort
                }
                break;
            /* End of Transmission. */
            case X_EOT:
            	uartTransmitChar(X_ACK);
            	status = X_DONE;
            	break;
            /* Abort from host. */
            case X_CAN:
           		status = X_ERROR;
           		break;
            default:
            /* Wrong header. */
            	status = xmodem_error_handler(&error_number, X_MAX_ERRORS);
            	break;
        }
    }
    if (status == X_DONE) return 1;
    else if (status == X_WRONG_FIRMWARE) return 2;
    else return 0;
}

/**
 * @brief   Calculates the CRC-16 for the input package.
 * @param   *data:  Array of the data which we want to calculate.
 * @param   length: Size of the data, either 128 or 1024 bytes.
 * @return  status: The calculated CRC.
 */
static uint16_t xmodemCalcCrc(uint8_t *data, uint16_t length)
{
    uint16_t crc = 0u;
    while (length)
    {
        length--;
        crc = crc ^ ((uint16_t)*data++ << 8u);
        for (uint8_t i = 0u; i < 8u; i++)
        {
            if (crc & 0x8000u)
            {
                crc = (crc << 1u) ^ 0x1021u;
            }
            else
            {
                crc = crc << 1u;
            }
        }
    }
    return crc;
}

/**
 * @brief   This function handles the data packet we get from the xmodem protocol.
 * @param   header: SOH or STX.
 * @return  status: Report about the packet.
 */
static xmodem_status xmodem_handle_packet(uint8_t header) {
	uint8_t dataTempBuf[128];
	uint8_t crcBuf[2];
    uint16_t size = 0;

    /* Receive packet number */
    size = uartReceive(xmodemBuf, 2);
    if (size != 2)
    	return X_ERROR_UART;
    if (xmodemBuf[0] != xmodemPacketNumber)
    	return X_ERROR_NUMBER;
    if (xmodemBuf[0] + xmodemBuf[1] != 255)
    	return X_ERROR_NUMBER;

    /* Receive packet data */
    size = uartReceive(dataTempBuf, X_PACKET_128_SIZE);
    if (size != X_PACKET_128_SIZE)
    	return X_ERROR_UART;

    /* Receive CRC */
    size = uartReceive(crcBuf, 2);
    if (size != 2)
    	return X_ERROR_UART;

    /* Check CRC */
    uint16_t crcMerged = ((uint16_t) crcBuf[0] << 8) | ((uint16_t) crcBuf[1]); //Merge the two bytes of CRC
    uint16_t crcCalculated = xmodemCalcCrc(dataTempBuf, X_PACKET_128_SIZE);
    if (crcMerged != crcCalculated)
    	return X_ERROR_CRC;

    /* Add remaining data from previous packet to current data */
    add_remaining_data_to_current_buf(dataTempBuf);

    /* Remove unfinished line from xmodemBuf and place it to remaining data */
    cut_buf();

    /* Move data to flash */
    uint8_t flashHexCode = flashHex(xmodemBuf, xmodemBufSize);
    if (flashHexCode == 0)
    	return X_ERROR_FLASH;
    if (flashHexCode == 2)
    	return X_WRONG_FIRMWARE;

    xmodemPacketNumber++;
    return X_OK;
}

/**
 * @brief   Handles the xmodem error.
 *          Raises the error counter, then if the number of the errors reached critical, do a graceful abort, otherwise send a NAK.
 * @param   *error_number:    Number of current errors (passed as a pointer).
 * @param   max_error_number: Maximal allowed number of errors.
 * @return  status: X_ERROR in case of too many errors, X_OK otherwise.
 */
static xmodem_status xmodem_error_handler(uint8_t *error_number, uint8_t max_error_number) {
	xmodem_status status = X_OK;
	/* Raise the error counter. */
	(*error_number)++;
	/* If the counter reached the max value, then abort. */
	if ((*error_number) >= max_error_number) {
		/* Graceful abort. */
		uartTransmitChar(X_CAN);
		uartTransmitChar(X_CAN);
		status = X_ERROR;
	} else { // Otherwise send a NAK for a repeat.
		uartTransmitChar(X_NAK);
		status = X_OK;
	}
	if (xmodemPacketNumber == 1 && status == X_OK) {
		HAL_TIM_Base_Start_IT(&htim6);
		timerWorking = 1;
	}
	return status;
}


void add_remaining_data_to_current_buf(uint8_t* receivedData) {
	uint8_t i;
	if (remainingSize != 0) {
		for (i = 0; i < remainingSize; i++) {
			xmodemBuf[i] = remainingBuf[i];
		}
	}
	xmodemBufSize = remainingSize;
	remainingSize = 0;

	for (i = 0; i < X_PACKET_128_SIZE; i++) {
		xmodemBuf[xmodemBufSize + i] = receivedData[i];
	}
	xmodemBufSize += X_PACKET_128_SIZE;
}

void cut_buf() {
	uint8_t i;
	uint8_t linePtr;

	for (i = xmodemBufSize - 1; i >= 0; i--) {
		if (xmodemBuf[i] == '\n') {
			linePtr = i;
			break;
		}
	}
	remainingSize = xmodemBufSize - linePtr - 1;
	if (remainingSize == 0) return;
	xmodemBufSize -= remainingSize;

	for (i = 0; i < remainingSize; i++) {
		remainingBuf[i] = xmodemBuf[(linePtr + 1) + i];
	}
}
