// for windows use -lws2_32
#ifndef OI_ADDRESS
#define OI_ADDRESS 1
#include "oi_os.h"
#include "oi_net.h"
#include "oi_types.h"
#include <string.h>

#ifndef OI_WIN
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
    a->ipv6.sin6_flowinfo = 0;
    a->ipv6.sin6_scope_id = 0;
    if (ip) a->ipv6.sin6_addr = *(struct in6_addr*)ip;
    else memset(&a->ipv6.sin6_addr,0,16);
    return 0;
}

oi_call address_from_name(address_t * a, const char * s, uint16 port, int lookup) {
    struct addrinfo hint, *res, *p;
    _OI_NET_INIT;
    
    memset(&hint,0,sizeof hint);
    hint.ai_family = AF_UNSPEC;
    if (!lookup) hint.ai_flags = AI_NUMERICHOST;
    
    if (getaddrinfo(s,0,&hint,&res)) return 1;
    for (p = res; p; p = p->ai_next) {
       if (p->ai_family == AF_INET || p->ai_family == AF_INET6) {
           a->raw = *p->ai_addr;
           a->ipv4.sin_port = htons(port);
           freeaddrinfo(res);
           _OI_NET_DEINIT;
           return 0;
        }
    }   
    freeaddrinfo(res);
    _OI_NET_DEINIT;
    return 1;
}

oi_call address_any(address_t * a, uint16 port) {
    a->ipv4.sin_family = AF_INET;
    a->ipv4.sin_port = htons(port);
    memset(&a->ipv4.sin_addr,0,4);
    return 0;
}

oi_call address_loopback(address_t * a, uint16 port) {
    const char ipv4_loopback[] = {127,0,0,1};
//    const char ipv6_loopback[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1};
    a->ipv4.sin_family = AF_INET;
    a->ipv4.sin_port = htons(port);
    memcpy(&a->ipv4.sin_addr,ipv4_loopback,4);
    return 0;
}

oi_call address_name(address_t * a, char * s, size_t len, int lookup) {
    _OI_NET_INIT;
    if (getnameinfo(&a->raw,sizeof(address_t),s,len,0,0,lookup ? 0 : NI_NUMERICHOST)) {
        _OI_NET_DEINIT;
        return 1;
    } else {
        _OI_NET_DEINIT;
        return 0;
    }
}

oi_func void * address_address(address_t * a, size_t * len) {
    if (a->raw.sa_family == AF_INET) {
        if (len) *len = 4;
        return &a->ipv4.sin_addr;
    } else {
        if (len) *len = 16;
        return &a->ipv6.sin6_addr;
    }
}

oi_func uint16 address_port(address_t * a) {
    return ntohs(a->ipv4.sin_port);
}

#endif
