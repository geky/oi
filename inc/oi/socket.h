// requires -lws2_32 for windows
#ifndef OI_SOCKET
#define OI_SOCKET 1

#include "oi/os.h"
#include "oi/types.h"
#include "oi/net.h"
#include "oi/address.h"

#include <string.h>

#ifdef OI_CPP
extern "C" {
#endif


// type socket_t //
#ifdef OI_WIN
typedef SOCKET _oi_sock;
#else
typedef signed int _oi_sock;
#endif

#ifdef OI_SINGLESTACK
typedef struct {
    _oi_sock ipv6;
    _oi_sock ipv4;
} socket_t;
#else
typedef union {
    _oi_sock ipv6;
    _oi_sock ipv4;
} socket_t;
#endif
///////////////////


// Definitions of socket types //
enum {
    SOCKET_UDP = SOCK_DGRAM,
    SOCKET_TCP = SOCK_STREAM
};
/////////////////////////////////


oi_call socket_create(socket_t *, int proto, uint16 port);

oi_call socket_create_on(socket_t *, int proto, address_t *);

oi_call socket_destroy(socket_t *);

oi_call socket_set_send_buffer(socket_t *, size_t len);

size_t socket_get_send_buffer(socket_t *);

oi_call socket_set_rec_buffer(socket_t *, size_t len);

size_t socket_get_rec_buffer(socket_t *);


#ifdef OI_CPP
}
#endif

#endif
