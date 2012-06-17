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

const char _oi_ipv4_any[] = {0,0,0,0};
#define IPV4_ANY (struct in_addr*)_oi_ipv4_any
const char _oi_ipv4_loopback[] = {127,0,0,1};
#define IPV4_lOOPBACK (struct in_addr*)_oi_ipv4_loopback

const char _oi_ipv6_any[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
#define IPV6_ANY (struct in6_addr*)_oi_ipv6_any
const char _oi_ipv6_loopback[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1};
#define IPV6_lOOPBACK (struct in6_addr*)_oi_ipv6_loopback

typedef	union {
	struct sockaddr raw;
	struct sockaddr_in  ipv4;
	struct sockaddr_in6 ipv6;
} address_t;

static inline int address_create_ipv4(address_t * a, void * ip, uint16 port) {
	a->ipv4.sin_family = AF_INET;
	a->ipv4.sin_port = htons(port);
	if (ip) a->ipv4.sin_address = *(struct in_addr*)ip;
	return 0;
}

static inline int address_scan_ipv4(address_t * a, const char * s) {
	return inet_pton(AF_INET,s,&a->ipv4.sin_addr) != -1;
}

static inline int address_create_ipv6(address_t * a, void * ip, uint16 port) {
	a->ipv6.sin_family = AF_INET6;
	a->ipv6.sin_port = htons(port);
	if (ip) a->ipv4.sin_address = *(struct in_addr6*)ip;
	return 0;
}

static inline int address_scan_ipv6(address_t * a, const char * s) {
	return inet_pton(AF_INET6,s,&a->ipv6.sin_addr) != -1;
}

static inline int address_lookup(address_t * a, const char * s) {
	struct addrinfo * result;
	if (getaddrinfo(s,0,0,&result)) return 1;
	a->raw = *(result->ai_addr);
	freeaddrinfo(result);
	return 0;
}

static inline int name_lookup(address_t * a, char * s, size_t len) {
	return getnameinfo(&a->raw,sizeof(address_t),s,len,0,0,0);
}

#endif
