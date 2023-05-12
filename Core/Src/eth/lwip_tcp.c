#include <eth/lwip_tcp.h>
#include <eth/tcp_protocol.h>
#include "string.h"

#if LWIP_TCP

static struct tcp_pcb *tpcb;

enum conn_states {
  CONN_NONE = 0,
  CONN_ACCEPTED,
  CONN_RECEIVED,
  CONN_CLOSING
};

struct conn_state {
  u8_t state;
  u8_t retries;
  struct tcp_pcb *pcb;
  /* pbuf (chain) to recycle */
  struct pbuf *p;
};

err_t accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err);
err_t recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
void error_callback(void *arg, err_t err);
err_t poll_callback(void *arg, struct tcp_pcb *tpcb);
err_t sent_callback(void *arg, struct tcp_pcb *tpcb, u16_t len);
void send(struct tcp_pcb *tpcb, struct conn_state *es);
void conn_close(struct tcp_pcb *tpcb, struct conn_state *es);

void lwip_tcp_init(void) {
  tpcb = tcp_new();
  if (tpcb != NULL) {
    err_t err;
	ip4_addr_t ip_addr;
	IP4_ADDR(&ip_addr, 169, 254, 8, 45);
    err = tcp_bind(tpcb, &ip_addr, 80);
    if (err == ERR_OK) {
      tpcb = tcp_listen(tpcb);
      tcp_accept(tpcb, accept_callback);
    } else {
      /* abort? output diagnostic? */
    }
  } else {
    /* abort? output diagnostic? */
  }
}


err_t accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err) {
  err_t ret_err;
  struct conn_state *es;

  LWIP_UNUSED_ARG(arg);
  LWIP_UNUSED_ARG(err);

  /* commonly observed practive to call tcp_setprio(), why? */
  tcp_setprio(newpcb, TCP_PRIO_MIN);

  es = (struct conn_state *)mem_malloc(sizeof(struct conn_state));
  if (es != NULL) {
    es->state = CONN_ACCEPTED;
    es->pcb = newpcb;
    es->retries = 0;
    es->p = NULL;
    /* pass newly allocated es to our callbacks */
    tcp_arg(newpcb, es);
    tcp_recv(newpcb, recv_callback);
    tcp_err(newpcb, error_callback);
    tcp_poll(newpcb, poll_callback, 0);
    ret_err = ERR_OK;
  } else {
    ret_err = ERR_MEM;
  }
  return ret_err;
}

err_t recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
  struct conn_state *es;
  err_t ret_err;

  LWIP_ASSERT("arg != NULL",arg != NULL);
  es = (struct conn_state *)arg;
  if (p == NULL) {
    /* remote host closed connection */
    es->state = CONN_CLOSING;
    if(es->p == NULL) {
       /* we're done sending, close it */
       conn_close(tpcb, es);
    } else {
      /* we're not done yet */
      tcp_sent(tpcb, sent_callback);
      send(tpcb, es);
    }
    ret_err = ERR_OK;
  } else if(err != ERR_OK) {
    /* cleanup, for unknown reason */
    if (p != NULL) {
      es->p = NULL;
      pbuf_free(p);
    }
    ret_err = err;
  } else if(es->state == CONN_ACCEPTED) {
    /* first data chunk in p->payload */
    es->state = CONN_RECEIVED;
    /* store reference to incoming pbuf (chain) */
    es->p = p;
    /* install send completion notifier */
    tcp_sent(tpcb, sent_callback);
    send(tpcb, es);
    ret_err = ERR_OK;
  } else if (es->state == CONN_RECEIVED) {
    /* read some more data */
    if(es->p == NULL) {
      es->p = p;
      tcp_sent(tpcb, sent_callback);
      send(tpcb, es);
    } else {
      struct pbuf *ptr;
      /* chain pbufs to the end of what we recv'ed previously  */
      ptr = es->p;
      pbuf_chain(ptr,p);
    }
    ret_err = ERR_OK;
  } else if(es->state == CONN_CLOSING) {
    /* odd case, remote side closing twice, trash data */
    tcp_recved(tpcb, p->tot_len);
    es->p = NULL;
    pbuf_free(p);
    ret_err = ERR_OK;
  } else {
    /* unkown es->state, trash data  */
    tcp_recved(tpcb, p->tot_len);
    es->p = NULL;
    pbuf_free(p);
    ret_err = ERR_OK;
  }
  return ret_err;
}

void error_callback(void *arg, err_t err) {
  struct conn_state *es;

  LWIP_UNUSED_ARG(err);

  es = (struct conn_state *)arg;
  if (es != NULL) {
    mem_free(es);
  }
}

err_t poll_callback(void *arg, struct tcp_pcb *tpcb) {
  err_t ret_err;
  struct conn_state *es;

  es = (struct conn_state *)arg;
  if (es != NULL) {
    if (es->p != NULL) {
      /* there is a remaining pbuf (chain)  */
      tcp_sent(tpcb, sent_callback);
      send(tpcb, es);
    } else {
      /* no remaining pbuf (chain)  */
      if(es->state == CONN_CLOSING) {
        conn_close(tpcb, es);
      }
    }
    ret_err = ERR_OK;
  } else {
    /* nothing to be done */
    tcp_abort(tpcb);
    ret_err = ERR_ABRT;
  }
  return ret_err;
}

err_t sent_callback(void *arg, struct tcp_pcb *tpcb, u16_t len) {
  struct conn_state *es;

  LWIP_UNUSED_ARG(len);

  es = (struct conn_state *)arg;
  es->retries = 0;

  if(es->p != NULL) {
    /* still got pbufs to send */
    tcp_sent(tpcb, sent_callback);
    send(tpcb, es);
  } else {
    /* no more pbufs to send */
    if(es->state == CONN_CLOSING) {
      conn_close(tpcb, es);
    }
  }
  return ERR_OK;
}

void send(struct tcp_pcb *tpcb, struct conn_state *es) {
  struct pbuf *ptr;
  err_t wr_err = ERR_OK;

  while ((wr_err == ERR_OK) &&
         (es->p != NULL) &&
         (es->p->len <= tcp_sndbuf(tpcb))) {
  ptr = es->p;

  /* enqueue data for transmission */
  char* answer = handle_packet(tpcb, ptr);
  if (answer != NULL) {
	  tcp_write(tpcb, answer, strlen(answer), TCP_WRITE_FLAG_COPY);
  }

  u16_t plen;
  u8_t freed;

  plen = ptr->len;
  /* continue with next pbuf in chain (if any) */
  es->p = ptr->next;
  if(es->p != NULL) {
     /* new reference! */
     pbuf_ref(es->p);
  } do { // chop first pbuf from chain
     /* try hard to free pbuf */
     freed = pbuf_free(ptr);
  } while(freed == 0);
     /* we can read more data now */
     tcp_recved(tpcb, plen);
  }
}

void conn_close(struct tcp_pcb *tpcb, struct conn_state *es) {
  tcp_arg(tpcb, NULL);
  tcp_sent(tpcb, NULL);
  tcp_recv(tpcb, NULL);
  tcp_err(tpcb, NULL);
  tcp_poll(tpcb, NULL, 0);

  if (es != NULL) {
    mem_free(es);
  }
  tcp_close(tpcb);
}

void conn_abort() {
	tcp_abort(tpcb);
}

void netifSetUp() {
	struct netif *netif;
	netif = netif_find("st0");
	netif_set_up(netif);
}

void netifSetDown() {
	struct netif *netif;
	netif = netif_find("st0");
	netif_set_down(netif);
}
#endif /* LWIP_TCP */
