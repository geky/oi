#ifndef OI_SOCKET
#define OI_SOCKET 1
#include "oi_os.h"
#include "oi_types.h"
#include "oi_net.h"
#include "oi_address.h"


#ifdef OI_WIN
#   define _OI_SOCK SOCKET
#   define _OI_SINVAL INVALID_SOCKET
#   define _OI_SCLOSE(sock) closesocket(sock)
#   define _OI_SBLOCK(sock) {\
        u_long blk = 1; \
        if(ioctlsocket(sock,FIONBIO,&blk)) \
            _OI_SDIE(5); \
    } 
#else
#   include <fcntl.h>
#   define _OI_SOCK signed int
#   define _OI_SINVAL -1
#   define _OI_SCLOSE(sock) close(sock)
#   define _OI_SBLOCK(sock) \
        if (fcntl(sock, F_SETFL, fcntl(sock, F_GETFL)&~O_NONBLOCK)) \
            _OI_SDIE(5);
#endif

#if defined(OI_DUALSTACK) 
#define _OI_STACK(sock) { \
    int opt = 0; \
    if (setsockopt(sock,IPPROTO_IPV6,IPV6_V6ONLY,&opt,sizeof opt)) \
        _OI_SDIE(4); \
}
#elif defined(OI_IPV6) && defined(IPV6_V6ONLY)
#define _OI_STACK(sock) { \
    int opt = 1; \
    if (setsockopt(sock,IPPROTO_IPV6,IPV6_V6ONLY,&opt,sizeof opt)) \
        _OI_SDIE(4); \
}
#else
#define _OI_STACK(sock)
#endif


enum {
    SOCKET_UDP = SOCK_DGRAM,
    SOCKET_TCP = SOCK_STREAM
};

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

oi_call socket_destroy(socket_t * s);
#define _OI_SDIE(rr) {socket_destroy(s); return rr;}
#define _OI_SMAP(addr) \
    address_t map; \
    memset(&map,0,sizeof map); \
    map.ipv6.sin6_family = AF_INET6; \
    map.ipv6.sin6_port = addr->ipv4.sin_port; \
    memset(&map.ipv6.sin6_addr.s6_addr[10],0xff,2); \
    memcpy(&map.ipv6.sin6_addr.s6_addr[12],&addr->ipv4.sin_addr,4);


oi_call socket_create(socket_t * s, int proto, uint16 port) {
    address_t temp;
    _OI_NET_INIT;
#if defined(OI_SINGLESTACK)
    s->ipv6 = _OI_SINVAL;
#endif
#if defined(OI_IPV4) || defined(OI_SINGLESTACK)
    s->ipv4 = socket(AF_INET, proto, 0);
    if (s->ipv4 == _OI_SINVAL) _OI_SDIE(-1);
    _OI_SBLOCK(s->ipv4); 
    
    if (port) {
        memset(&temp,0,sizeof temp.ipv4);
        temp.ipv4.sin_family = AF_INET;
        temp.ipv4.sin_port = htons(port);
        if (bind(s->ipv4, &temp.raw, sizeof temp.ipv4)) _OI_SDIE(7);
    }
#endif
#if defined(OI_IPV6) || defined(OI_DUALSTACK) || defined(OI_SINGLESTACK)
    s->ipv6 = socket(AF_INET6, proto, 0);
    if (s->ipv6 == _OI_SINVAL) _OI_SDIE(-1);
    _OI_STACK(s->ipv6);
    _OI_SBLOCK(s->ipv6);

    if (port) {
        memset(&temp,0,sizeof temp.ipv6);
        temp.ipv6.sin6_family = AF_INET6;
        temp.ipv6.sin6_port = htons(port);
        if (bind(s->ipv6, &temp.raw, sizeof temp.ipv6)) _OI_SDIE(7);
    }
#endif
    return s->ipv6 == _OI_SINVAL;
}

oi_call socket_create_address(socket_t * s, int proto, address_t * a) {
    if (a->family == AF_INET) {
#if defined(OI_IPV4) || defined(OI_SINGLESTACK)
        _OI_NET_INIT;
#if defined(OI_SINGLESTACK)
        s->ipv6 = _OI_SINVAL;
#endif
        s->ipv4 = socket(AF_INET, proto, 0);
        if (s->ipv4 == _OI_SINVAL) _OI_SDIE(-1);

        _OI_SBLOCK(s->ipv4); 
        if (bind(s->ipv4, &a->raw, sizeof a->ipv4)) _OI_SDIE(7);
#elif defined(OI_DUALSTACK)
        _OI_NET_INIT;
        s->ipv6 = socket(AF_INET6, proto, 0);
        if (s->ipv6 == _OI_SINVAL) {_OI_NET_DEINIT; return -1;}

        _OI_STACK(s->ipv6);
        _OI_SBLOCK(s->ipv6);
        {   _OI_SMAP(a) 
            if (bind(s->ipv6, &map.raw, sizeof map.ipv6)) _OI_SDIE(7);
        }
#else
        s->ipv4 = _OI_SINVAL;
        return 10;
#endif    
    } else {
#if defined(OI_IPV6) || defined(OI_DUALSTACK) || defined(OI_SINGLESTACK)
        _OI_NET_INIT;
#if defined(OI_SINGLESTACK)
        s->ipv4 = _OI_SINVAL;
#endif
        s->ipv6 = socket(AF_INET6, proto, 0);
        if (s->ipv6 == _OI_SINVAL) _OI_SDIE(-1);
        _OI_STACK(s->ipv6);
        _OI_SBLOCK(s->ipv6);
        if (bind(s->ipv6, &a->raw, sizeof a->ipv6)) _OI_SDIE(7);
#else
        s->ipv6 = _OI_SINVAL;
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

oi_call tcp_connect(socket_t * s, address_t * a) {
    if (a->family == AF_INET) {
#if defined(OI_IPV4)
        return connect(s->ipv4, &a->raw, sizeof a->ipv4);
#elif defined(OI_SINGLESTACK)
        if (connect(s->ipv4, &a->raw, sizeof a->ipv4)) return -1;
        if (s->ipv6 != _OI_SINVAL) {
            _OI_SCLOSE(s->ipv6);
            s->ipv6 = _OI_SINVAL;
        }
        return 0;
#elif defined(OI_DUALSTACK)
        _OI_SMAP(a)
        return connect(s->ipv6, &map.raw, sizeof map.ipv6);
#else
        return 10;
#endif
    } else {
#if defined(OI_IPV6) || defined(OI_DUALSTACK)
        return connect(s->ipv6, &a->raw, sizeof a->ipv6);
#elif defined(OI_SINGLESTACK)
        if (connect(s->ipv6, &a->raw, sizeof a->ipv6)) return -1;
        if (s->ipv4 != _OI_SINVAL) {
            _OI_SCLOSE(s->ipv4);
            s->ipv4 = _OI_SINVAL;
        }
        return 0;
#else
        return 10;
#endif
    }
}
#if 0
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
