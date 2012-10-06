// requires -lws2_32 for windows
#ifndef OI_UDP
#define OI_UDP 1

#include "oi_os.h"
#include "oi_types.h"
#include "oi_net.h"
#include "oi_address.h"
#include "oi_socket.h"

#ifdef OI_CPP
extern "C" {
#endif


oi_call udp_send(socket_t *, void * buf, size_t * len, address_t * a);

oi_call udp_rec(socket_t *, void * buf, size_t * len, address_t * na);

oi_call udp_rec_any(socket_t ** res, void * buf, size_t * len, address_t * na, unsigned int ms, int num, ...);


#ifdef OI_CPP
}
#endif

#endif
