#ifndef INC_LWIP_CALLBACKS_H_
#define INC_LWIP_CALLBACKS_H_

#include "err.h"
#include "arch.h"
#include "pbuf.h"
#include "tcp.h"

err_t tcp_accept_callback(void* arg, struct tcp_pcb* newpcb, err_t err);

#endif /* INC_LWIP_CALLBACKS_H_ */
