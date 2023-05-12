#ifndef INC_PROTOCOL_H_
#define INC_PROTOCOL_H_

#include "flash.h"
#include "stdbool.h"
#include "lwip_tcp.h"

/* Maximum allowed errors (user defined). */
#define X_MAX_ERRORS ((uint8_t)3u) //TODO max errors

/* Bytes defined by the protocol. */
#define ACK    ((uint8_t)0x79u)  /* Acknowledge */
#define NACK   ((uint8_t)0x1Fu)  /* Not Acknowledge */
#define EOT    ((uint8_t)0xF5u)  /* End of transmission */
#define JUMP   ((uint8_t)0x21u)  /* Jump to app */
#define UPDATE ((uint8_t)0x63u)  /* Download new app */
#define VER    ((uint8_t)0x02u)  /* Get versions of apps */
#define HELP   ((uint8_t)0x10u)  /* Get list with commands description*/
#define CLR    ((uint8_t)0x43u)  /* Erase config sectors (clear all apps) */

typedef enum {
  CODE,
  DATA
} transfer_status;

char* handle_packet(struct tcp_pcb* tpcb, struct pbuf* p);

#endif /* INC_PROTOCOL_H_ */
