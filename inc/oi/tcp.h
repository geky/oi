#ifndef OI_TCP
#define OI_TCP 1

#include "oi/os.h"
#include "oi/types.h"
#include "oi/net.h"
#include "oi/address.h"
#include "oi/socket.h"

#include <stdarg.h>

#ifdef OI_CPP
extern "C" {
#endif


// requires "-Lws2_32" on windows
//
// oi_tcp contains the socket code specific to TCP, 
// the Transmission Control Protocol.


// Connects the specified socket to the given address. If an error is returned 
// the socket will be cleaned up completely and require reinitialization.
// 
// It can return ERR_REFUSED if the reciever does not accept the connection,
// ERR_UNREACHABLE_HOST if the end computer is not reachable,
// ERR_UNREACHABLE_NET if the network containing the end computer unreachable,
// and ERR_TIMEOUT if the connection times out.
oi_call tcp_connect(socket_t *, address_t *);

// Waits for a connection on any of the specified sockets to any of 
// the addresses given or times out after the given milliseconds. 
// The socket and address that make a connection will be pointed to by 
// res_s and res_a if they are not null. All sockets that don't make a 
// connection will be cleaned up completely and require reinitialization.
//
// It can return ERR_REFUSED if the reciever does not accept the connection, 
// ERR_UNREACHABLE_HOST if the end computer is not reachable, 
// ERR_UNREACHABLE_NET if the network containing the end computer unreachable, 
// and ERR_TIMEOUT if the connection times out. 
oi_call tcp_connect_any(socket_t ** res_s, address_t ** res_a, unsigned int ms, int num, ...);

// Waits for a new connection on the specified socket. 
// When a new connection occurs, ns is created with the new connection and 
// the address that is being connected from is stored in na if it is not null.
oi_call tcp_accept(socket_t *, socket_t * ns, address_t * na);

// Waits for a new connection on any of the specified sockets
// for the time given in milliseconds. 
//
// If a new connection occurs, ns is created with the new connection, and 
// the address that is being connected from is stored in addr if it is not null.
// res will then point to the socket that was connected to if it was not null.
//
// Otherwise it returns ERR_TIMEOUT.
oi_call tcp_accept_any(socket_t ** res, socket_t * ns, address_t * na, unsigned int ms, int num, ...);

// Sends the buffer of data through the socket given through TCP. 
// The actual amount sent is stored in len, and should only differ from the 
// given size if an error occured.
// 
// It can return ERR_DISCONNECTED if the socket is disconnected, 
// or ERR_TIMEOUT if the connection times out. 
oi_call tcp_send(socket_t *, void * buf, size_t * len);

// Waits for data to be received on the socket. If the socket receives data 
// it is stored in the buffer limited by the length given in len, and the new 
// amount of data is stored in len. 
// 
// It can return ERR_DISCONNECTED if the socket is disconnected, 
// or ERR_TIMEOUT if the connection times out.
oi_call tcp_rec(socket_t *, void * buf, size_t * len);

// Waits for data to be received on any of the specified sockets 
// for the milliseconds given. 
//
// If a socket receives data it is stored in the buffer limited by the length 
// given in len, and the new amount of data stored in len. res will then point
// to the socket that recieved data if it is not null. 
// 
// Otherwise it will return ERR_TIMEOUT and res and len will be set to 0.
//
// It can also return ERR_DISCONNECTED or ERR_TIMEOUT prematurely if one 
// of the sockets becomes disconnected.
oi_call tcp_rec_any(socket_t ** res, void * buf, size_t * len, unsigned int ms, int num, ...);

// Enables or disables nodelay on the socket. 
// Normally a socket will wait for a decently sized buffer to fill up before 
// sending a packet in tcp. setting the val to non-zero will enable 
// nodelay and send data immediatly.
oi_call tcp_set_nodelay(socket_t * s, int val);

// Gets whether or not the current socket has the nodelay option enabled.
int tcp_get_nodelay(socket_t * s);

// Enables or disables keepalive on the socket give. 
// If these are enabled, the socket will send out keepalive packets to insure
// that the connection is connected. If a connection becomes unconnected, 
// ERR_DISCONNECTED is returned on any recieve or send function. 
// A disconnected state can still be determined without keepalive enabled as 
// long as there is a data transmission. 
//
// The time interval is usually around 2 hours, so it isn't very useful unless
// you will be connected for long periods of time without sending any data.
// Setting val non-zero will enable keepalive packets.
oi_call tcp_set_keepalive(socket_t * s, int val);

// Gets whether or not the current socket has the keepalive option enabled.
int tcp_get_keepalive(socket_t * s);


#ifdef OI_CPP
}
#endif

#endif
