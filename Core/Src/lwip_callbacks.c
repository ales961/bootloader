#include "lwip_callbacks.h"
#include "usart.h"


err_t tcp_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
	uint16_t data_len = p->len;
	tcp_write(tpcb, p->payload, data_len, 0);
	uartTransmit(p->payload, data_len);
	tcp_recved(tpcb, data_len); //ret = tcp_recved(tpcb, data_len);
	/* Process the incoming TCP packet */
	//if (p->len >= sizeof(my_protocol_header_t)) {
	//    my_protocol_header_t* header = (my_protocol_header_t*)p->payload;

	    /* Extract the payload data */
	//    char* payload_data = p->payload + sizeof(my_protocol_header_t);
	//    int payload_len = p->len - sizeof(my_protocol_header_t);

	    /* Process the payload data */
	    // ...

	    /* Send a response over the TCP connection */
	//    char data[] = "Hello, world!";
	//    tcp_write(tcp_pcb, data, sizeof(data), 1);
	//}

	/* Free the packet buffer */
	pbuf_free(p);

	return ERR_OK;
}

void tcp_error_callback(void *arg, err_t err) {
	uartTransmit("error tcp", 9);
}

err_t tcp_poll_callback(void *arg, struct tcp_pcb *tpcb) {
	return ERR_OK;
}

err_t tcp_sent_callback(void *arg, struct tcp_pcb *tpcb, u16_t len) {
	//tpcb->snd_buf = NULL;
	return ERR_OK;
}

err_t tcp_accept_callback(void* arg, struct tcp_pcb* newpcb, err_t err) {
	/* initialize lwIP tcp_recv callback function for newpcb */
	tcp_recv(newpcb, tcp_recv_callback);
	/* initialize lwIP tcp_err callback function for newpcb */
	tcp_err(newpcb, tcp_error_callback);
	/* initialize lwIP tcp_poll callback function for newpcb */
	tcp_poll(newpcb, tcp_poll_callback, 1);
	/* initialize lwIP tcp_sent callback function for newpcb */
	tcp_sent(newpcb, tcp_sent_callback);
	return ERR_OK;
}

