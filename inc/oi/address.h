#ifndef OI_ADDRESS
#define OI_ADDRESS 1

#include "oi/os.h"
#include "oi/types.h"
#include "oi/net.h"

#include <string.h>
#ifndef OI_WIN
#include <arpa/inet.h>
#endif

#ifdef OI_CPP
extern "C" {
#endif


// requires "-Lws2_32" on windows
//
// oi_address contains code for using internet addresses.
// Each address contains an IP address as well as a port number and 
// internal data for the internet protocol.


// type address_t //
typedef union {
    unsigned short family;
    struct sockaddr raw;
    struct sockaddr_in  ipv4;
    struct sockaddr_in6 ipv6;
} address_t;
////////////////////


// Creates an address from the raw IPv4 address and port. 
// The address must be IPv4 and have a size of 4 bytes.
oi_call address_from_ipv4(address_t *, void * ip, uint16 port);

//Creates an address from the raw IPv6 address and port. 
// The address must be IPv6 and have a size of 16 bytes.
oi_call address_from_ipv6(address_t *, void * ip, uint16 port);

// Creates an address from the null-terminated string representation and port. 
// If the value of lookup is not zero, a DNS lookup is performed for 
// domain names, otherwise only conversion from the standard numeric 
// notation of IP addresses can be performed. 
// 
// It can return either ERR_NOT_FOUND if name is not found, or 
// ERR_NO_DATA if the name is found but has no data associated with it
oi_call address_from_name(address_t *, const char * s, uint16 port, int lookup);

// Multiple IP addresses can exist for a single domain name. 
// address_from_name_all allows you to retrieve all address representations 
// of a name in an array pointed to by adarr with length given in len. 
// It then sets the value of len to the number of addresses found. 
//
// It can return either ERR_NOT_FOUND if name is not found, or 
// ERR_NO_DATA if the name is found but has no data associated with it
oi_call address_from_name_all(address_t * adarr, size_t * len, const char * s, uint16 port, int lookup);

// Creates a loopback address with the port given independent of the IP version.
oi_call address_loopback(address_t *, uint16 port);

// Creates an address containing the address of the host currently stored 
// in the system and with the port provided.
oi_call address_host(address_t *, uint16 port);

// Puts a null-terminated string representation of the address into the buffer
// provided, limited by the buffer size in len. 
// If lookup is not zero a DNS lookup occurs to determine the domain name, 
// otherwise a numeric representation is used.
oi_call address_name(address_t *, char * string, size_t len, int lookup);

// Gets the numeric representation of the underlying IP address
void * address_address(address_t *, size_t * len);

// Gets the port number
uint16 address_port(address_t *);


#ifdef OI_CPP
}
#endif

#endif
