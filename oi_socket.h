#ifndef OI_SOCKET
#define OI_SOCKET 1
#include "oi_os.h"
#include "oi_types.h"
#include "oi_net.h"
#include "oi_address.h"

#ifdef OI_DUALSTACK
#   define _OI_SDSTACK(sock,val) {\
        int opt = val;            \
        if (setsockopt(sock,IPPROTO_IPV6,IPV6_V6ONLY,&opt,sizeof opt))\
            return 4;             \
    }
#else
#   define _OI_SDSTACK(sock,val)
#endif

#ifdef OI_WIN
#
#   define _OI_SINVAL INVALID_SOCKET
#   define _OI_SCLOSE(sock) closesocket(sock)
#   define _OI_SBLOCK(sock,val) {\
        u_long blk = val?0:1;    \
        if (ioctlsocket(sock,FIONBIO,&blk)) return 3;\
    }

typedef struct {
    SOCKET ipv4;
    SOCKET ipv6;
} socket_t;

#else
#   include <fcntl.h>
#
#   define _OI_SINVAL -1
#   define _OI_SCLOSE(sock) close(sock)
#   define _OI_SBLOCK(sock,val) \
        if (block) fcntl(sock, F_SETFL, fcntl(sock, F_GETFL)&~O_NONBLOCK); 

typedef struct {
    signed int ipv4;
    signed int ipv6;
} socket_t;

#endif


oi_call socket_create(socket_t * s, int proto, int block) { 
    _OI_NET_INIT;
    s->ipv6 = socket(IPV6, proto, 0);
    _OI_SBLOCK(s->ipv6,block);
    
#ifdef OI_DUALSTACK
    s->ipv4 = s->ipv6;
    _OI_SDSTACK(s->ipv6, 0);
#else
    s->ipv4 = socket(IPV4, proto, 0);
    _OI_SBLOCK(s->ipv4,block);
#endif
    return s->ipv6 == _OI_SINVAL;
}

oi_call socket_create_ipv4(socket_t * s, int proto, int block) {
    _OI_NET_INIT;
    s->ipv4 = socket(IPV4, proto, 0);
    s->ipv6 = _OI_SINVAL;
    _OI_SBLOCK(s->ipv4, block);
    return s->ipv4 == _OI_SINVAL;
}

oi_call socket_create_ipv6(socket_t * s, int proto, int block) {
    _OI_NET_INIT;
    s->ipv6 = socket(IPV6, proto, 0);
    s->ipv4 = _OI_SINVAL;
    _OI_SBLOCK(s->ipv6, block);
    _OI_SDSTACK(s->ipv6, 1);
    return s->ipv6 == _OI_SINVAL;
}

oi_call socket_destroy(socket_t * s) {
    if (s->ipv6 != _OI_SINVAL) _OI_SCLOSE(s->ipv6);
    if (s->ipv4 != _OI_SINVAL && s->ipv4 != s->ipv6) _OI_SCLOSE(s->ipv4);
    _OI_NET_DEINIT;
    return 0;
}

oi_call socket_bind(socket_t * s, uint16 port) {
    address_t temp;

    if (s->ipv6 != _OI_SINVAL) {
        memset(&temp,0,sizeof temp);
        temp.ipv6.sin6_family = IPV6;
        temp.ipv6.sin6_port = htons(port);
        if (bind(s->ipv6, &temp.raw, sizeof temp.ipv6)) return 6;
    }
    
    if (s->ipv4 != _OI_SINVAL && s->ipv4 != s->ipv6) {
        memset(&temp,0,sizeof temp);
        temp.ipv4.sin_family = IPV4;
        temp.ipv4.sin_port = htons(port);
        if (bind(s->ipv4, &temp.raw, sizeof temp.ipv4)) return 4;
    }
    return 0;
}

oi_call socket_bind_address(socket_t * s, address_t * a) {
    if (a->family == IPV6 && s->ipv6 != _OI_SINVAL) {
        return bind(s->ipv6, &a->raw, sizeof a->ipv6);
    } else if (a->family == IPV4 && s->ipv4 != _OI_SINVAL) {
        if (s->ipv4 != s->ipv6) {
            return bind(s->ipv4, &a->raw, sizeof a->ipv4);
        } else {
            address_t temp;
            memset(&temp,0,sizeof temp);
            temp.ipv6.sin6_family = IPV6;
            temp.ipv6.sin6_port = a->ipv4.sin_port;
            temp.ipv6.sin6_addr.s6_addr[10] = 0xff;
            temp.ipv6.sin6_addr.s6_addr[11] = 0xff;
            memcpy(&temp.ipv6.sin6_addr.s6_addr[12],&a->ipv4.sin_addr,4);

            return bind(s->ipv6, &temp.raw, sizeof temp.ipv6);
        }
    }

    return -1;
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
