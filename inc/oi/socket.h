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


// requires "-Lws2_32" for windows
//
// oi_socket contains code dealing with the use of internet sockets. 
// Sockets can be created independently of underlying protocol, but to 
// actually send data you will need to use either oi_tcp or oi_udp for their 
// respective protocols. 
//
// Sockets are complex entities, so it is encouraged to research each protocol 
// yourself to understand how they they work.


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


// Creates an internet socket. The type must be provided in proto and can
// either be SOCKET_TCP or SOCKET_UDP depending on the protocol that is needed.
// The socket is then bound to the port given or, if port is 0, it will be 
// bound to a system determined port when nescessary. 
//
// socket_create can return ERR_TAKEN if the given port is already in use.
oi_call socket_create(socket_t *, int proto, uint16 port);

// Creates an internet socket. The type must be provided in proto and can
// either be SOCKET_TCP or SOCKET_UDP depending on the protocol that is needed.
// The socket is then bound to the specific address given by addr. 
//
// socket_create_on can return ERR_TAKEN if the given port is already in use.
oi_call socket_create_on(socket_t *, int proto, address_t *);

// Closes any connections used and cleans up any resources that were used.
oi_call socket_destroy(socket_t *);

// Sets the socket's send-buffer size to the length give.
oi_call socket_set_send_buffer(socket_t *, size_t len);

// Gets the socket's send-buffer size.
size_t socket_get_send_buffer(socket_t *);

// Sets the socket's receive-buffer size to the length give.
oi_call socket_set_rec_buffer(socket_t *, size_t len);

// Gets the socket's receive-buffer size.
size_t socket_get_rec_buffer(socket_t *);


#ifdef OI_CPP
}
#endif

#endif
