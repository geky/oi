// requires -lws2_32 for windows
#ifndef OI_TCP
#define OI_TCP 1

#include "oi_os.h"
#include "oi_types.h"
#include "oi_net.h"
#include "oi_address.h"
#include "oi_socket.h"

#ifdef OI_CPP
extern "C" {
#endif


oi_call tcp_connect(socket_t *, address_t *);

oi_call tcp_connect_any(socket_t ** res_s, address_t ** res_a, unsigned int ms, int num, ...);

oi_call tcp_accept(socket_t *, socket_t * ns, address_t * na);

oi_call tcp_accept_any(socket_t ** res_s, socket_t * ns, address_t * na, unsigned int ms, int num, ...);

oi_call tcp_send(socket_t *, void * buf, size_t * len);

oi_call tcp_rec(socket_t *, void * buf, size_t * len);

oi_call tcp_rec_any(socket_t ** res_s, void * buf, size_t * len, unsigned int ms, int num, ...);

oi_call tcp_set_nodelay(socket_t * s, int val);

oi_call tcp_set_keepalive(socket_t * s, int val);

int tcp_get_nodelay(socket_t * s);

int tcp_get_keepalive(socket_t * s);


#ifdef OI_CPP
}
#endif

#endif
