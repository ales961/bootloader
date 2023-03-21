#include <stdlib.h>
#include <string.h>
#include "tcp_protocol.h"
#include "flash.h"
#include "hex_parser.h"
#include "boot_config.h"

/* Global variables. */
uint8_t flashBuf[1024];
uint16_t flashBufSize = 0;
uint8_t remainingDataBuf[128];
uint16_t remainingDataSize = 0;
transfer_status tr_status = CODE;


/* Local functions. */
void addRemainingDataToCurrentBuf(uint8_t* receivedData, uint16_t length);
void cutBuf();

char* handle_packet(struct tcp_pcb* tpcb, struct pbuf* p) {
	if (p == NULL || p->len <= 0 || p->payload == NULL) return "handle error";
	uint8_t header;
	uint8_t* data = NULL;
	uint16_t length = p->len - 1;
	memcpy(&header, p->payload, 1);
	if (length > 0) {
		data = malloc(sizeof(uint8_t) * length);
		memcpy(data, p->payload + 1, length);
	}
	if (tr_status == CODE) {
		switch (header) {
			case JUMP:
				if(!data) free(data);
				validateApplications();
				if (getLatestApplicationAddress() == 0) {
					return "No valid application in flash\n";
				} else {
					jumpToApp();
					return "Done";
				}
			case UPDATE:
				validateApplications();
				updateConfig();
				setAppVersion((uint32_t) data[0]);//TODO
				free(data);
				tr_status = DATA;
				return "Send hex file: \n";
			case VER:
				if(!data) free(data);
				validateApplications();
				return getVersions();
			case HELP:
				if(!data) free(data);
				return "jump: jump to application\
				\nupdate <version>: download firmware and jump to it\
				\nversion: get current version of application\
				\nhelp: get information about commands\
				\nclear: erase configs\n";
			case CLR:
				if(!data) free(data);
				EraseSector(CONFIG_1_SECTOR);
				EraseSector(CONFIG_2_SECTOR);
				return "Configs erased\n";
			default:
				if(!data) free(data);
				return "No such command\n";
		}
	} else if (tr_status == DATA) {
		if (header == EOT) {
			tr_status = CODE;
			setCorrectUpdateFlag();
			jumpToApp();
			return "Done";
		}
	    /* Add remaining data from previous packet to current data */
		if (length <= 0) return "data empty error";
		addRemainingDataToCurrentBuf(data, length);

	    /* Remove unfinished line from flashBuf and place it to remaining data */
	    cutBuf();

	    /* Move data to flash */
	    uint8_t flashHexCode = flashHex(flashBuf, flashBufSize);
	    uint8_t answer;
	    if (flashHexCode == 0) { //Flash write error   TODO
	    	answer = NACK;
	    	tcp_write(tpcb, &answer, 1, 1);
	    	tcp_output(tpcb);
	    	return NULL;
	    }
	    if (flashHexCode == 2) { //Wrong bank firmware error
	    	answer = NACK;
	    	tcp_write(tpcb, &answer, 1, 1);
	    	tcp_output(tpcb);
	    	return NULL;
	    }

	    answer = ACK;
		tcp_write(tpcb, &answer, 1, 1);
		tcp_output(tpcb); //TODO
		free(data);
		return NULL;
	}
	return "unknown error";
}

void addRemainingDataToCurrentBuf(uint8_t* receivedData, uint16_t length) {
	uint16_t i;
	if (remainingDataSize != 0) {
		for (i = 0; i < remainingDataSize; i++) {
			flashBuf[i] = remainingDataBuf[i];
		}
	}
	flashBufSize = remainingDataSize;
	remainingDataSize = 0;

	for (i = 0; i < length; i++) {
		flashBuf[flashBufSize + i] = receivedData[i];
	}
	flashBufSize += length;
}

void cutBuf() {
	uint16_t i;
	uint16_t linePtr;

	for (i = flashBufSize - 1; i >= 0; i--) {
		if (flashBuf[i] == '\n') {
			linePtr = i;
			break;
		}
	}
	remainingDataSize = flashBufSize - linePtr - 1;
	if (remainingDataSize == 0) return;
	flashBufSize -= remainingDataSize;

	for (i = 0; i < remainingDataSize; i++) {
		remainingDataBuf[i] = flashBuf[(linePtr + 1) + i];
	}
}
