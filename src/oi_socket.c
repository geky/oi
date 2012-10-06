#include "oi_socket.h"

#ifdef OI_WIN
#   define _OI_SERR_NOS WSAEAFNOSUPPORT
#   define _OI_SERR_TIME ERROR_TIMEOUT
#   define _OI_SERR_PROG WSAEWOULDBLOCK
#   define _OI_SERR_CONN WSAEISCONN
#   define _OI_SERR_DISC WSAECONNRESET
#   define _OI_TIME_ERR (_OI_NET_ERR==WSAETIMEDOUT?_OI_SERR_TIME:_OI_NET_ERR)
#   define _OI_SINVAL INVALID_SOCKET
#   define _OI_SCLOSE(sock) closesocket(sock)
#   define _OI_SBLOCK(sock) { \
        u_long blk = 0; \
        if (ioctlsocket(sock,FIONBIO,&blk)) \
            _OI_SDIE(_OI_NET_ERR,s); \
    } 
#   define _OI_SUNBLOCK(sock) { \
        u_long blk = 1; \
        if (ioctlsocket(sock,FIONBIO,&blk)) \
            _OI_SDIE(_OI_NET_ERR,s); \
    }
#else
#   define _OI_SERR_NOS EAFNOSUPPORT
#   define _OI_SERR_TIME ETIMEDOUT
#   define _OI_SERR_PROG EINPROGRESS
#   define _OI_SERR_CONN EISCONN
#   define _OI_SERR_DISC ECONNRESET
#   define _OI_TIME_ERR _OI_NET_ERR
#   define _OI_SINVAL -1
#   define _OI_SCLOSE(sock) close(sock)
#   define _OI_SBLOCK(sock) \
        if (fcntl(sock, F_SETFL, fcntl(sock, F_GETFL)&~O_NONBLOCK)) \
            _OI_SDIE(_OI_NET_ERR,s);
#   define _OI_SUNBLOCK(sock) \
        if (fcntl(sock, F_SETFL, fcntl(sock, F_GETFL)|O_NONBLOCK)) \
            _OI_SDIE(_OI_NET_ERR,s);
#endif


#if defined(OI_DUALSTACK) 
#define _OI_SOPT6(sock) { \
    int opt = 0; \
    if (setsockopt(sock,IPPROTO_IPV6,IPV6_V6ONLY,(char*)&opt,sizeof opt)) \
        _OI_SDIE(_OI_NET_ERR,s); \
    opt = 1; \
    if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(char*)&opt,sizeof opt)) \
        _OI_SDIE(_OI_NET_ERR,s); \
}
#else
#define _OI_SOPT4(sock) { \
    int opt = 1; \
    if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(char*)&opt,sizeof opt)) \
        _OI_SDIE(_OI_NET_ERR,s); \
}
#if defined(IPV6_V6ONLY)
#define _OI_SOPT6(sock) { \
    int opt = 1; \
    if (setsockopt(sock,IPPROTO_IPV6,IPV6_V6ONLY,(char*)&opt,sizeof opt)) \
        _OI_SDIE(_OI_NET_ERR,s); \
    if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(char*)&opt,sizeof opt)) \
        _OI_SDIE(_OI_NET_ERR,s); \
}
#else
#define _OI_SOPT6(sock) _OI_SOPT4(sock)
#endif
#endif


#define _OI_SDIE(rr,ss) {int ee=rr; socket_destroy(ss); return ee;}
#define _OI_SMAP(addr,map) \
    memset(&map,0,sizeof map); \
    map.ipv6.sin6_family = AF_INET6; \
    map.ipv6.sin6_port = addr->ipv4.sin_port; \
    memset(&map.ipv6.sin6_addr.s6_addr[10],0xff,2); \
    memcpy(&map.ipv6.sin6_addr.s6_addr[12],&addr->ipv4.sin_addr,4);
#define _OI_SUMAP(addr) \
    if (addr && addr->ipv6.sin6_addr.s6_addr[0] == 0x00 && \
                addr->ipv6.sin6_addr.s6_addr[1] == 0x00 && \
                addr->ipv6.sin6_addr.s6_addr[2] == 0x00 && \
                addr->ipv6.sin6_addr.s6_addr[3] == 0x00 && \
                addr->ipv6.sin6_addr.s6_addr[4] == 0x00 && \
                addr->ipv6.sin6_addr.s6_addr[5] == 0x00 && \
                addr->ipv6.sin6_addr.s6_addr[6] == 0x00 && \
                addr->ipv6.sin6_addr.s6_addr[7] == 0x00 && \
                addr->ipv6.sin6_addr.s6_addr[8] == 0x00 && \
                addr->ipv6.sin6_addr.s6_addr[9] == 0x00 && \
                addr->ipv6.sin6_addr.s6_addr[10]== 0xff && \
                addr->ipv6.sin6_addr.s6_addr[11]== 0xff) { \
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
    if (s->ipv4 == _OI_SINVAL) _OI_SDIE(_OI_NET_ERR,s);
    _OI_SOPT4(s->ipv4);
    _OI_SBLOCK(s->ipv4);
    
    if (port) {
        memset(&temp,0,sizeof temp.ipv4);
        temp.ipv4.sin_family = AF_INET;
        temp.ipv4.sin_port = htons(port);
        if (bind(s->ipv4, &temp.raw, sizeof temp.ipv4)) _OI_SDIE(_OI_NET_ERR,s);
    }
#endif
#if defined(OI_IPV6) || defined(OI_DUALSTACK) || defined(OI_SINGLESTACK)
    s->ipv6 = socket(AF_INET6, proto, 0);
    if (s->ipv6 == _OI_SINVAL) _OI_SDIE(_OI_NET_ERR,s);
    _OI_SOPT6(s->ipv6);
    _OI_SBLOCK(s->ipv6);

    if (port) {
        memset(&temp,0,sizeof temp.ipv6);
        temp.ipv6.sin6_family = AF_INET6;
        temp.ipv6.sin6_port = htons(port);
        if (bind(s->ipv6, &temp.raw, sizeof temp.ipv6)) _OI_SDIE(_OI_NET_ERR,s);
    }
#endif
    return 0;
}


oi_call socket_create_on(socket_t * s, int proto, address_t * a) {
    if (a->family == AF_INET) {
#if defined(OI_IPV4) || defined(OI_SINGLESTACK)
        _OI_NET_INIT;
#if defined(OI_SINGLESTACK)
        s->ipv6 = _OI_SINVAL;
#endif
        s->ipv4 = socket(AF_INET, proto, 0);
        if (s->ipv4 == _OI_SINVAL) _OI_SDIE(_OI_NET_ERR,s);
        _OI_SOPT4(s->ipv4);
        _OI_SBLOCK(s->ipv4);
        if (bind(s->ipv4, &a->raw, sizeof a->ipv4)) _OI_SDIE(_OI_NET_ERR,s);
#elif defined(OI_DUALSTACK)
        _OI_NET_INIT;
        s->ipv6 = socket(AF_INET6, proto, 0);
        if (s->ipv6 == _OI_SINVAL) _OI_SDIE(_OI_NET_ERR,s);
        _OI_SOPT6(s->ipv6);
        _OI_SBLOCK(s->ipv6);
        {   address_t map;
            _OI_SMAP(a,map) 
            if (bind(s->ipv6, &map.raw, sizeof map.ipv6)) _OI_SDIE(_OI_NET_ERR,s);
        }
#else
        s->ipv4 = _OI_SINVAL;
        return _OI_SERR_NOS;
#endif    
    } else {
#if defined(OI_IPV6) || defined(OI_DUALSTACK) || defined(OI_SINGLESTACK)
        _OI_NET_INIT;
#if defined(OI_SINGLESTACK)
        s->ipv4 = _OI_SINVAL;
#endif
        s->ipv6 = socket(AF_INET6, proto, 0);
        if (s->ipv6 == _OI_SINVAL) _OI_SDIE(_OI_NET_ERR,s);
        _OI_SOPT6(s->ipv6);
        _OI_SBLOCK(s->ipv6);
        if (bind(s->ipv6, &a->raw, sizeof a->ipv6)) _OI_SDIE(_OI_NET_ERR,s);
#else
        s->ipv6 = _OI_SINVAL;
        return _OI_SERR_NOS;
#endif
    }
    return 0;
}


oi_call socket_destroy(socket_t * s) {
    int iserr = 0;
#if defined(OI_SINGLESTACK)
    if (s->ipv4 != _OI_SINVAL) {
        iserr |= _OI_SCLOSE(s->ipv4);
        s->ipv4 = _OI_SINVAL;
    }
#endif
    if (s->ipv6 != _OI_SINVAL) {
        iserr |= _OI_SCLOSE(s->ipv6);
        s->ipv6 = _OI_SINVAL;
    }
    _OI_NET_DEINIT;
    return iserr ? _OI_NET_ERR : 0;
}


oi_call socket_set_send_buffer(socket_t * s, size_t len) {
#if defined(OI_SINGLESTACK)    
    if (s->ipv4!=_OI_SINVAL && setsockopt(s->ipv4,
                           SOL_SOCKET,SO_SNDBUF,(char*)&len,sizeof len))
        return _OI_NET_ERR;
#endif
    if (s->ipv6!=_OI_SINVAL && setsockopt(s->ipv6,
                           SOL_SOCKET,SO_SNDBUF,(char*)&len,sizeof len))
        return _OI_NET_ERR;
    return 0;
}


oi_call socket_set_rec_buffer(socket_t * s, size_t len) {
#if defined(OI_SINGLESTACK)
    if (s->ipv4!=_OI_SINVAL && setsockopt(s->ipv4,
                           SOL_SOCKET,SO_RCVBUF,(char*)&len,sizeof len))
        return _OI_NET_ERR;
#endif
    if (s->ipv6!=_OI_SINVAL && setsockopt(s->ipv6,
                           SOL_SOCKET,SO_RCVBUF,(char*)&len,sizeof len))
        return _OI_NET_ERR;
    return 0;
}


size_t socket_get_send_buffer(socket_t * s) {
    size_t len = 0;
    socklen_t llen = sizeof len;
#if defined(OI_SINGLESTACK)
    getsockopt(s->ipv6==_OI_SINVAL ? s->ipv4 : s->ipv6,
                       SOL_SOCKET,SO_SNDBUF,(char*)&len,&llen);
#else
    getsockopt(s->ipv6,SOL_SOCKET,SO_SNDBUF,(char*)&len,&llen);
#endif
    return len;
}


size_t socket_get_rec_buffer(socket_t * s) {
    size_t len = 0;
    socklen_t llen = sizeof len;
#if defined(OI_SINGLESTACK)
    getsockopt(s->ipv6==_OI_SINVAL ? s->ipv4 : s->ipv6,
                       SOL_SOCKET,SO_RCVBUF,(char*)&len,&llen);
#else
    getsockopt(s->ipv6,SOL_SOCKET,SO_RCVBUF,(char*)&len,&llen);
#endif
    return len;
}

