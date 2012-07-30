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
#define _OI_SOPT(sock) { \
    const char opt = 0; \
    if (setsockopt(sock,IPPROTO_IPV6,IPV6_V6ONLY,&opt,sizeof opt)) \
        _OI_SDIE(4); \
    opt = 1;
    if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof opt)) \
        _OI_SDIE(6); \
}
#elif defined(OI_IPV6) && defined(IPV6_V6ONLY)
#define _OI_SOPT(sock) { \
    const char opt = 1; \
    if (setsockopt(sock,IPPROTO_IPV6,IPV6_V6ONLY,&opt,sizeof opt)) \
        _OI_SDIE(4); \
    if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof opt)) \
        _OI_SDIE(6); \
}
#else
#define _OI_SOPT(sock) { \
    const char opt = 1; \
    if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof opt)) \
        _OI_SDIE(6); \
}
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
#define _OI_SMAP(addr,map) \
    memset(&map,0,sizeof map); \
    map.ipv6.sin6_family = AF_INET6; \
    map.ipv6.sin6_port = addr->ipv4.sin_port; \
    memset(&map.ipv6.sin6_addr.s6_addr[10],0xff,2); \
    memcpy(&map.ipv6.sin6_addr.s6_addr[12],&addr->ipv4.sin_addr,4);
#define _OI_SUMAP(addr) \
    if (addr->ipv6.sin6_addr.s6_addr[0] == 0x00 && \
        addr->ipv6.sin6_addr.s6_addr[1] == 0x00 && \
        addr->ipv6.sin6_addr.s6_addr[2] == 0x00 && \
        addr->ipv6.sin6_addr.s6_addr[3] == 0x00 && \
        addr->ipv6.sin6_addr.s6_addr[4] == 0x00 && \
        addr->ipv6.sin6_addr.s6_addr[5] == 0x00 && \
        addr->ipv6.sin6_addr.s6_addr[6] == 0x00 && \
        addr->ipv6.sin6_addr.s6_addr[7] == 0x00 && \
        addr->ipv6.sin6_addr.s6_addr[8] == 0x00 && \
        addr->ipv6.sin6_addr.s6_addr[9] == 0x00 && \
        addr->ipv6.sin6_addr.s6_addr[10]== 0x00 && \
        addr->ipv6.sin6_addr.s6_addr[11]== 0xff && \
        addr->ipv6.sin6_addr.s6_addr[12]== 0xff) { \
        addr->ipv4.sin_family = AF_INET; \
        memcpy(&addr->ipv4.sin_addr, &addr->ipv6.sin6_addr.s6_addr[12], 4); \
    }


oi_call socket_create(socket_t * s, int proto, uint16 port) {
    address_t temp;
    _OI_NET_INIT;
#if defined(OI_SINGLESTACK)
    s->ipv6 = _OI_SINVAL;
#endif
#if defined(OI_IPV4) || defined(OI_SINGLESTACK)
    s->ipv4 = socket(AF_INET, proto, 0);
    if (s->ipv4 == _OI_SINVAL) _OI_SDIE(-1);
    _OI_SOPT(s->ipv4);
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
    _OI_SOPT(s->ipv6);
    _OI_SBLOCK(s->ipv6);

    if (port) {
        memset(&temp,0,sizeof temp.ipv6);
        temp.ipv6.sin6_family = AF_INET6;
        temp.ipv6.sin6_port = htons(port);
        if (bind(s->ipv6, &temp.raw, sizeof temp.ipv6)) _OI_SDIE(7);
    }
#endif
    return 0;
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
        _OI_SOPT(s->ipv4);
        _OI_SBLOCK(s->ipv4);
        if (bind(s->ipv4, &a->raw, sizeof a->ipv4)) _OI_SDIE(7);
#elif defined(OI_DUALSTACK)
        _OI_NET_INIT;
        s->ipv6 = socket(AF_INET6, proto, 0);
        if (s->ipv6 == _OI_SINVAL) _OI_SDIE(-1);
        _OI_SOPT(s->ipv6);
        _OI_SBLOCK(s->ipv6);
        {   address_t map;
            _OI_SMAP(a,map) 
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
        _OI_SOPT(s->ipv6);
        _OI_SBLOCK(s->ipv6);
        if (bind(s->ipv6, &a->raw, sizeof a->ipv6)) _OI_SDIE(7);
#else
        s->ipv6 = _OI_SINVAL;
        return 10;
#endif
    }
    return 0;
}

oi_call socket_destroy(socket_t * s) {
#if defined(OI_SINGLESTACK)
    if (s->ipv4 != _OI_SINVAL) _OI_SCLOSE(s->ipv4);
#endif
    if (s->ipv6 != _OI_SINVAL) _OI_SCLOSE(s->ipv6);
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
        address_t map;
        _OI_SMAP(a,map)
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


oi_call tcp_accept(socket_t * s, socket_t * ns, address_t * na) {
    int na_s = sizeof(address_t);
#if defined(OI_SINGLESTACK)    
    fd_set fset;
    
    if (listen(s->ipv4,5)) return 2;
#endif
    
    if (listen(s->ipv6,5)) return 2;
    
#if defined(OI_SINGLESTACK)      
    FD_ZERO(&fset);
    FD_SET(s->ipv4,&fset);
    FD_SET(s->ipv6,&fset);
    select(s->ipv4>s->ipv6 ? s->ipv4 : s->ipv6, &fset, 0, 0, 0);
    if (FD_ISSET(s->ipv4,&fset)) {
        ns->ipv4 = accept(s->ipv4, &na->raw, &na_s);
        ns->ipv6 = _OI_SINVAL;
        return ns->ipv4 == _OI_SINVAL;
    } else {
        ns->ipv6 = accept(s->ipv6, &na->raw, &na_s);
        ns->ipv4 = _OI_SINVAL;
        return ns->ipv6 == _OI_SINVAL;
    }        
#else
    ns->ipv6 = accept(s->ipv6,&na->raw,&na_s);
#if defined(OI_DUALSTACK)
    _OI_SUMAP(na);
#endif
    return ns->ipv6 == _OI_SINVAL;
#endif
}

oi_call tcp_send(socket_t * s, void * buf, size_t * len) {
    size_t sendlen = *len;
    size_t newlen;
    *len = 0;
    while(*len < sendlen) {
#if defined(OI_SINGLESTACK)
        newlen = send(s->ipv6==_OI_SINVAL?s->ipv4:s->ipv6,buf+*len,sendlen-*len,0);
#else
        newlen = send(s->ipv6,buf+*len,sendlen-*len,0);
#endif
        if (newlen < 0) return -1;
        *len += newlen;
    }
    return 0;
}

oi_call tcp_rec(socket_t * s, void * buf, size_t * len) {
#if defined(OI_SINGLESTACK)
    *len = recv(s->ipv6==_OI_SINVAL?s->ipv4:s->ipv6,buf,*len,0);
#else
    *len = recv(s->ipv6,buf,*len,0);
#endif
    if (*len < 0) {
        *len = 0;
        return -1;
    } return 0;
}

oi_call udp_send(socket_t * s, void * buf, size_t * len, address_t * a) {
    if (a->family == AF_INET) {
#if defined(OI_IPV4) || defined(OI_SINGLESTACK)
        *len = sendto(s->ipv4, buf, *len, 0, &a->raw, sizeof a->ipv4);
        if (*len < 0) {
            *len = 0;
            return -1;
        } return 0;
        
#elif defined(OI_DUALSTACK)
        address_t map;
        _OI_SMAP(a,map)
        *len = sendto(s->ipv6, buf, *len, 0, &map->raw, sizeof map->ipv6);
        if (*len < 0) {
            *len = 0;
            return -1;
        } return 0;
#else
        return 10;
#endif
    } else {
#if defined(OI_IPV6) || defined(OI_DUALSTACK) || defined(OI_SINGLESTACK)
        *len = sendto(s->ipv6, buf, *len, 0, &a->raw, sizeof a->ipv6);
        if (*len < 0) {
            *len = 0;
            return -1;
        } return 0;
#else
        return 10;
#endif
    }
}

oi_call udp_rec(socket_t * s, void * buf, size_t * len, address_t * na) {
    int na_s = sizeof(address_t);
#if defined(OI_SINGLESTACK)
    fd_set fset;
    FD_ZERO(&fset);
    FD_SET(s->ipv4,&fset);
    FD_SET(s->ipv6,&fset);
    select(s->ipv4>s->ipv6 ? s->ipv4 : s->ipv6, &fset, 0, 0, 0);
    if (FD_ISSET(s->ipv4,&fset)) {
        *len = recvfrom(s->ipv4, buf, *len, 0, &na->raw, &na_s);
    } else {
        *len = recvfrom(s->ipv6, buf, *len, 0, &na->raw, &na_s);
    }        
#else
    *len = recvfrom(s->ipv6, buf, *len, 0, &na->raw, &na_s);
#if defined(OI_DUALSTACK)
    _OI_SUMAP(na);
#endif
#endif
    if (*len < 0) {
        *len = 0;
        return -1;
    } return 0;
}

#endif
