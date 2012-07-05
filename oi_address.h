// for windows use -lws2_32
#ifndef OI_ADDRESS
#define OI_ADDRESS 1
#include "oi_os.h"
#include "oi_types.h"

#ifdef OI_WIN
#include <winsock2.h> 
#else
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#endif

typedef union {
    struct sockaddr raw;
    struct sockaddr_in  ipv4;
    struct sockaddr_in6 ipv6;
} address_t;

oi_call address_from_ipv4(address_t * a, void * ip, uint16 port) {
    a->ipv4.sin_family = AF_INET;
    a->ipv4.sin_port = htons(port);
    if (ip) a->ipv4.sin_addr = *(struct in_addr*)ip;
    else memset(&a->ipv4.sin_addr,0,4);
    return 0;
}

oi_call address_from_ipv6(address_t * a, void * ip, uint16 port) {
    a->ipv6.sin6_family = AF_INET6;
    a->ipv6.sin6_port = htons(port);
    if (ip) a->ipv6.sin6_addr = *(struct in6_addr*)ip;
    else memset(&a->ipv6.sin6_addr,0,16);
    return 0;
}

oi_call address_from_name(address_t * a, const char * s, uint16 port, int lookup) {
    if (inet_pton(AF_INET, s,&a->ipv4.sin_addr)) {
        a->ipv4.sin_family = AF_INET;
        a->ipv4.sin_port = htons(port);
        return 0;
    } else if (inet_pton(AF_INET6,s,&a->ipv6.sin6_addr)) {
        a->ipv6.sin6_family = AF_INET6;
        a->ipv6.sin6_port = htons(port);
        return 0;
    } else if (lookup) {
        struct addrinfo * result;
        if (getaddrinfo(s,0,0,&result)) return 1;
        a->raw = *(result->ai_addr);
        freeaddrinfo(result);
        a->ipv4.sin_port = htons(port);
        return 0;
    } else {
        return 1;
    }
}

oi_call address_any(address_t * a, uint16 port) {
    a->ipv4.sin_family = AF_INET;
    a->ipv4.sin_port = htons(port);
    memset(&a->ipv4.sin_addr,0,4);
    return 0;
}

oi_call address_localhost(address_t * a, uint16 port) {
    const char ipv4_loopback[] = {127,0,0,1};
//    const char ipv6_loopback[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1};
    a->ipv4.sin_family = AF_INET;
    a->ipv4.sin_port = htons(port);
    memcpy(&a->ipv4.sin_addr,ipv4_loopback,4);
    return 0;
}

oi_call address_get_name(address_t * a, char * s, size_t len, int lookup) {
    if (lookup) return getnameinfo(&a->raw,sizeof(address_t),s,len,0,0,0);
    if (a->raw.sa_family == AF_INET)
        inet_ntop(AF_INET,&a->ipv4.sin_addr,s,len);
    else
        inet_ntop(AF_INET6,&a->ipv6.sin6_addr,s,len);
    return 0;
}

oi_func void * address_get_address(address_t * a, size_t * len) {
    if (a->raw.sa_family == AF_INET) {
        if (len) *len = 4;
        return &a->ipv4.sin_addr;
    } else {
        if (len) *len = 16;
        return &a->ipv6.sin6_addr;
    }
}

oi_func uint16 address_get_port(address_t * a) {
    return ntohs(a->ipv4.sin_port);
}

#endif
