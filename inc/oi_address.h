// requires -lws2_32 for windows
#ifndef OI_ADDRESS
#define OI_ADDRESS 1

#include "oi_os.h"
#include "oi_types.h"
#include "oi_net.h"

#ifdef OI_CPP
extern "C" {
#endif


// type address_t

typedef union {
    unsigned short family;
    struct sockaddr raw;
    struct sockaddr_in  ipv4;
    struct sockaddr_in6 ipv6;
} address_t;


oi_call address_from_ipv4(address_t *, void * ip, uint16 port);

oi_call address_from_ipv6(address_t *, void * ip, uint16 port);

oi_call address_from_name(address_t *, const char * s, uint16 port, int lookup);

oi_call address_from_name_all(address_t *, size_t * len, const char * s, uint16 port, int lookup);

oi_call address_loopback(address_t *, uint16 port);

oi_call address_host(address_t *, uint16 port);

oi_call address_name(address_t *, char * string, size_t len, int lookup);

void * address_address(address_t *, size_t * len);

uint16 address_port(address_t *);


#ifdef OI_CPP
}
#endif

#endif
