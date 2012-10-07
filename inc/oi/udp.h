#ifndef OI_UDP
#define OI_UDP 1

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
// oi_udp contains the socket code specific to UDP, 
// the User Datagram Protocol.


// Sends the buffer of data through the socket to the address specified. 
// It uses the UDP protocol and the packet that is sent has no garuntee 
// for delivery. The amount sent is then stored in len and may be less 
// than the amount given if it can not fit in a UDP packet.
oi_call udp_send(socket_t *, void * buf, size_t * len, address_t * a);

// Waits for data to be received on the socket. When the socket receives data 
// it is stored in the buffer limited by the length given in len, and the 
// new amount of data is stored in len. The address that it is received from
// is then stored in addr if it is not null.
oi_call udp_rec(socket_t *, void * buf, size_t * len, address_t * na);

// Waits for data to be received on any of the udp sockets given for the 
// amount of milliseconds given. 
//
// If the socket receives data it is stored in the buffer limited by the 
// length given in len, and the new amount of data is stored in len. 
// The address that the data is received from is then stored in addr if it 
// is not null, and the socket that receives the data is pointed to by res if
// it is not null. 
//
// If the time runs out, len and select are set to 0 and 
// the functions returns ERR_TIMEOUT.
oi_call udp_rec_any(socket_t ** res, void * buf, size_t * len, address_t * na, unsigned int ms, int num, ...);


#ifdef OI_CPP
}
#endif

#endif
