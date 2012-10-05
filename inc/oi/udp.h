// requires -lws2_32 for windows
#ifndef OI_UDP
#define OI_UDP 1

#include "oi/os.h"
#include "oi/types.h"
#include "oi/net.h"
#include "oi/address.h"
#include "oi/socket.h"

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
