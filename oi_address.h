// requires -lws2_32 for windows
#ifndef OI_ADDRESS
#define OI_ADDRESS 1
#include "oi_os.h"
#include "oi_types.h"
#include "oi_net.h"

#if defined(OI_IPV4)
#   define _OI_AFAMILY AF_INET
#elif defined(OI_IPV6)
#   define _OI_AFAMILY AF_INET6
#else
#   define _OI_AFAMILY AF_UNSPEC
#endif

#ifdef OI_WIN
#define _OI_RGAI {_OI_NET_DEINIT; return err;}
#else
#   if EAI_FAMILY < 0
#       define _OI_RGAI return err;
#   else
#       define _OI_RGAI return -err;
#   endif
#endif

typedef union {
    unsigned short family;
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
    memset(a,0,sizeof(address_t));
    a->ipv6.sin6_family = AF_INET6;
    a->ipv6.sin6_port = htons(port);
    if (ip) a->ipv6.sin6_addr = *(struct in6_addr*)ip;
    return 0;
}

oi_call address_from_name(address_t * a, const char * s, uint16 port, int lookup) {
    struct addrinfo hint, *res;
    int err;
    _OI_NET_INIT;
    
    memset(&hint,0,sizeof hint);
    hint.ai_family = _OI_AFAMILY;
    if (!lookup) hint.ai_flags = AI_NUMERICHOST;
    
    if ((err = getaddrinfo(s,0,&hint,&res))) _OI_RGAI;
    memcpy(a,res->ai_addr,res->ai_addrlen);
    a->ipv4.sin_port = htons(port);
    
    freeaddrinfo(res);
    _OI_NET_DEINIT;
    return 0;
}

oi_call address_all_from_name(address_t * a, size_t * len, const char * s, uint16 port, int lookup) {
    size_t t=0;
    struct addrinfo hint, *res, *hit;
    int err;
    _OI_NET_INIT;
    
    memset(&hint,0,sizeof hint);
    hint.ai_family = _OI_AFAMILY;
    hint.ai_socktype = SOCK_STREAM;
    if (!lookup) hint.ai_flags = AI_NUMERICHOST;
    
    if ((err = getaddrinfo(s,0,&hint,&res))) _OI_RGAI;
    for (hit = res; t < *len && hit; t++, hit = hit->ai_next) { 
        memcpy(&a[t],hit->ai_addr,hit->ai_addrlen);
        a[t].ipv4.sin_port = htons(port);
    }

    *len = t;
    freeaddrinfo(res);
    _OI_NET_DEINIT;
    return 0;
}

oi_call address_loopback(address_t * a, uint16 port) {
    struct addrinfo hint, *res;
    int err;
    _OI_NET_INIT;
    
    memset(&hint,0,sizeof hint);
    hint.ai_family =_OI_AFAMILY;
    
    if ((err = getaddrinfo("localhost",0,&hint,&res))) _OI_RGAI;
    memcpy(a,res->ai_addr,res->ai_addrlen);
    a->ipv4.sin_port = htons(port);
    
    freeaddrinfo(res);
    _OI_NET_DEINIT;
    return 0;
}

oi_call address_host(address_t * a, uint16 port) {
    struct addrinfo hint, *res;
    char name[256];
    int err;
    _OI_NET_INIT;

    if (gethostname(name,sizeof name)) {_OI_NET_DEINIT; return _OI_NET_ERR;}
    memset(&hint,0,sizeof hint);
    hint.ai_family = _OI_AFAMILY;

    if ((err = getaddrinfo(name,0,&hint,&res))) _OI_RGAI;
    memcpy(a,res->ai_addr,res->ai_addrlen);
    a->ipv4.sin_port = htons(port);
    
    freeaddrinfo(res);
    _OI_NET_DEINIT;
    return 0;
}

oi_call address_name(address_t * a, char * s, size_t len, int lookup) {
    int err;
    _OI_NET_INIT;
    err = getnameinfo(&a->raw,sizeof(address_t),s,len,0,0,lookup?0:NI_NUMERICHOST);
    _OI_RGAI;
}

oi_func void * address_address(address_t * a, size_t * len) {
    if (a->family == AF_INET) {
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
