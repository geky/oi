#ifndef OI_SOCKET
#define OI_SOCKET 1
#include "oi_os.h"
#include "oi_types.h"
#include "oi_net.h"
#include "oi_address.h"

#define SOCKET_UDP SOCK_DGRAM
#define SOCKET_TCP SOCK_STREAM

#ifdef OI_WIN
#   define _OI_SOCK SOCKET
#   define _OI_SINVAL INVALID_SOCKET
#   define _OI_SCLOSE(sock) closesocket(sock)
#   define _OI_SBLOCK(sock,val) {\
        u_long blk = val?0:1;    \
        if (ioctlsocket(sock,FIONBIO,&blk)) return 3;\
    }
#else
#   include <fcntl.h>
#   define _OI_SOCK signed int
#   define _OI_SINVAL -1
#   define _OI_SCLOSE(sock) close(sock)
#   define _OI_SBLOCK(sock,val) \
        if (block) fcntl(sock, F_SETFL, fcntl(sock, F_GETFL)&~O_NONBLOCK);
#endif

#define _OI_SMAP(addr) \
    address_t map; \
    memset(&map,0,sizeof map); \
    map.ipv6.sin6_family = AF_INET6; \
    map.ipv6.sin6_port = addr->ipv4.sin_port; \
    memset(&map.ipv6.sin6_addr.s6_addr[10],0xff,2); \
    memcpy(&map.ipv6.sin6_addr.s6_addr[12],&addr->ipv4.sin_addr,4);

#if defined(OI_DUALSTACK) 
#define _OI_STACK(sock) { \
    int opt = 0; \
    if (setsockopt(sock,IPPROTO_IPV6,IPV6_V6ONLY,&opt,sizeof opt)) \
        return 4; \
}
#elif defined(OI_IPV6) && defined(IPV6_V6ONLY)
#define _OI_STACK(sock) { \
    int opt = 1; \
    if (setsockopt(sock,IPPROTO_IPV6,IPV6_V6ONLY,&opt,sizeof opt)) \
        return 4; \
}
#else
#define _OI_STACK(sock)
#endif

#ifdef OI_SINGLESTACK
typedef struct {
    _OI_SOCK ipv6;
    _OI_SOCK ipv4;
} socket_t;
#else
typedef union {
    _OI_SOCK ipv6;
    _OI_SOCK ipv4;
} socket_t;
#endif

oi_call socket_create(socket_t * s, int proto, uint16 port, int block) {
    address_t temp;
   _OI_NET_INIT;
#if defined(OI_IPV4) || defined(OI_SINGLESTACK)
    s->ipv4 = socket(AF_INET, proto, 0);
    _OI_SBLOCK(s->ipv4,block); 
    
    if (port) {
        memset(&temp,0,sizeof temp.ipv4);
        temp.ipv4.sin_family = AF_INET;
        temp.ipv4.sin_port = htons(port);
        if (bind(s->ipv4, &temp.raw, sizeof temp.ipv4)) return 7;
    }
#endif
#if defined(OI_IPV6) || defined(OI_DUALSTACK) || defined(OI_SINGLESTACK)
    s->ipv6 = socket(AF_INET6, proto, 0);
    _OI_STACK(s->ipv6);
    _OI_SBLOCK(s->ipv6,block);

    if (port) {
        memset(&temp,0,sizeof temp.ipv6);
        temp.ipv6.sin6_family = AF_INET6;
        temp.ipv6.sin6_port = htons(port);
        if (bind(s->ipv6, &temp.raw, sizeof temp.ipv6)) return 7;
    }
#endif
    return s->ipv6 == _OI_SINVAL;
}

oi_call socket_create_address(socket_t * s, int proto, address_t * a, int block) {
    _OI_NET_INIT;
    if (a->family == AF_INET) {
#if defined(OI_IPV4) || defined(OI_SINGLESTACK)
        s->ipv4 = socket(AF_INET, proto, 0);
        _OI_SBLOCK(s->ipv4,block); 
        if (bind(s->ipv4, &a->raw, sizeof a->ipv4)) return 7;
#if defined(OI_SINGLESTACK)
        s->ipv6 = _OI_SINVAL;
#endif
#elif defined(OI_DUALSTACK)
        s->ipv6 = socket(AF_INET6, proto, 0);
        _OI_STACK(s->ipv6);
        _OI_SBLOCK(s->ipv6,block);
        { _OI_SMAP(a) if (bind(s->ipv6, &map.raw, sizeof map.ipv6)) return 7; }
#else
        return 10;
#endif    
    } else {
#if defined(OI_IPV6) || defined(OI_DUALSTACK) || defined(OI_SINGLESTACK)
        s->ipv6 = socket(AF_INET6, proto, 0);
#if defined(OI_SINGLESTACK)
        s->ipv4 = _OI_SINVAL;
#endif
        _OI_STACK(s->ipv6);
        _OI_SBLOCK(s->ipv6,block);
        if (bind(s->ipv6, &a->raw, sizeof a->ipv6)) return 7;
#else
        return 10;
#endif
    }
    return s->ipv6 == _OI_SINVAL && s->ipv4 == _OI_SINVAL;
}

oi_call socket_destroy(socket_t * s) {
#if defined(OI_IPV4) || defined(OI_SINGLESTACK)
    if (s->ipv4 != _OI_SINVAL) _OI_SCLOSE(s->ipv4);
#endif
#if defined(OI_IPV6) || defined(OI_DUALSTACK) || defined(OI_SINGLESTACK)
    if (s->ipv6 != _OI_SINVAL) _OI_SCLOSE(s->ipv6);
#endif
    _OI_NET_DEINIT;
    return 0;
}

#if 0
oi_call tcp_connect(socket_t * s, address_t * a) {
    return connect(*s,(sockaddr*)a,sizeof(address_t));
}

oi_call tcp_accept(socket_t * s, socket_t * ns, address_t * na) {
    int na_s = sizeof(address_t);
    if (listen(*s,5)) return 2;
    *ns = accept(*s,(sockaddr*)na,&na_s);
    return (*ns == -1);
}

oi_call tcp_send(socket_t * s, void * buf, size_t len) {
    return send(*s,buf,len,0) == -1;
}

oi_call tcp_rec(socket_t * s, void * buf, size_t len, size_t * inlen) {
    ssize_t rsize = recv(*s,buf,len,0);
    if (rsize >= 0) *inlen = rsize;
    return rsize < 0;
}

oi_call udp_send(socket_t * s, void * buf, size_t len, address_t * a) {
    return sendto(*s,buf,len,0,(sockaddr*)a,sizeof(address_t)) == -1;
}

oi_call udp_rec(socket_t * s, void * buf, size_t len, size_t * inlen, address_t * a) {
    int a_s = sizeof(address_t);
    ssize_t rsize = recvfrom(*s,buf,len,0,(sockaddr*)a,&a_s);
    if (rsize >= 0 && inlen) *inlen = rsize;
    return rsize < 0;
}


#endif
#endif
