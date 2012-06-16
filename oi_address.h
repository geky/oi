// for windows use -lws2_32

#ifndef OI_ADDRESS
#define OI_ADDRESS
#include "oi_os.h"
#include "oi_types.h"

#ifdef OI_WIN
#include <winsock2.h> 
#else
#include <sys/socket.h>
#include <string.h>
#include <fcntl.h>
#endif

#define ADDRESS_ANY       INADDR_ANY
#define ADDRESS_lOOPBACK  INADDR_LOOPBACK
#define ADDRESS_BROADCAST INADDR_BROADCAST
#define ADDRESS_NONE      INADDR_NONE

typedef	union {
	struct sockaddr_in  ipv4;
	struct sockaddr_in6 ipv6;
} address_t;

static inline int address_4_create(address_t * a, uint32 ip, uint16 port) {
	a->ipv4.sin_family = AF_INET;
	a->ipv4.sin_port = htons(port);
	a->ipv4.sin_address = (struct in_addr)htonl(ip);
	return 0;
}

static inline int address_6_create(address_t * a, uint32 ip, uint16 port) {
	a->ipv6.sin_family = AF_INET6;
	a->ipv6.sin_port = htons(port);
	a->ipv4.sin_address = (struct in_addr6)htonl(ip);
	return 0;
}

#endif
