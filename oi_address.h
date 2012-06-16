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

static inline int ipv4_address_create(address_t * a, unsigned char * ip, uint16 port) {
	a->ipv4.sin_family = AF_INET;
	a->ipv4.sin_port = htons(port);
	if (ip) a->ipv4.sin_address = *(struct in_addr *)ip;
	return 0;
}

static inline int ipv4_address_create(address_t * a, char * s) {
	return inet_pton(AF_INET,s,&a->ipv4.sin_addr) != -1;
}

static inline int ipv6_address_create(address_t * a, unsigned char * ip, uint16 port) {
	a->ipv6.sin_family = AF_INET6;
	a->ipv6.sin_port = htons(port);
	if (ip) a->ipv4.sin_address = *(struct in_addr6 *)ip;
	return 0;
}

static inline int ipv6_address_create(address_t * a, char * s) {
	return inet_pton(AF_INET6,s,&a->ipv6.sin_addr) != -1;
}

#endif
