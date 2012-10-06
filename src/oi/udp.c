#include "oi/udp.h"

#ifdef OI_WIN
#   define _OI_NET_INIT {                       \
        WSADATA wsadata; int err; \
        if((err = WSAStartup(MAKEWORD(2,2),&wsadata)))  \
            return err; }

#   define _OI_NET_DEINIT                       \
        if (WSACleanup()) return WSAGetLastError();

#   define _OI_NET_ERR WSAGetLastError()
#
#
#   define _OI_SERR_NOS WSAEAFNOSUPPORT
#   define _OI_SERR_TIME ERROR_TIMEOUT
#   define _OI_SINVAL INVALID_SOCKET
#
#else
#   include <errno.h>
#
#   define _OI_NET_INIT
#   define _OI_NET_DEINIT
#   define _OI_NET_ERR errno
#
#   define _OI_SERR_NOS EAFNOSUPPORT
#   define _OI_SERR_TIME ETIMEDOUT
#   define _OI_SINVAL -1
#
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



oi_call udp_send(socket_t * s, void * buf, size_t * len, address_t * a) {
    int newlen = *len;
    *len = 0;

    if (a->family == AF_INET) {
#if defined(OI_IPV4) || defined(OI_SINGLESTACK)
        newlen = sendto(s->ipv4, buf, newlen, 0, &a->raw, sizeof a->ipv4);
        if (newlen < 0) return _OI_NET_ERR;
        *len = newlen; 
        return 0;
#elif defined(OI_DUALSTACK)
        address_t map;
        _OI_SMAP(a,map)
        newlen = sendto(s->ipv6, buf, newlen, 0, &map.raw, sizeof map.ipv6);
        if (newlen < 0) return _OI_NET_ERR;
        *len = newlen; 
        return 0;
#else
        return _OI_SERR_NOS;
#endif
    } else {
#if defined(OI_IPV6) || defined(OI_DUALSTACK) || defined(OI_SINGLESTACK)
        newlen = sendto(s->ipv6, buf, newlen, 0, &a->raw, sizeof a->ipv6);
        if (newlen < 0) return _OI_NET_ERR;
        *len = newlen; 
        return 0;
#else
        return _OI_SERR_NOS;
#endif
    }
}


oi_call udp_rec(socket_t * s, void * buf, size_t * len, address_t * na) {
    socklen_t na_s = sizeof(address_t);
    address_t dump;
    int newlen = *len;
    *len = 0;
    
#if defined(OI_SINGLESTACK)
    if (s->ipv4 == _OI_SINVAL) {
        newlen = recvfrom(s->ipv6, buf, newlen,0, na?&na->raw:&dump.raw, &na_s);
    } else if (s->ipv6 == _OI_SINVAL) {
        newlen = recvfrom(s->ipv4, buf, newlen,0, na?&na->raw:&dump.raw, &na_s);
    } else {
        fd_set fset;
        FD_ZERO(&fset);
        FD_SET(s->ipv4,&fset);
        FD_SET(s->ipv6,&fset);
        select((s->ipv4>s->ipv6?s->ipv4:s->ipv6)+1, &fset,0,0,0);
        
        if (FD_ISSET(s->ipv4,&fset)) {
            newlen = recvfrom(s->ipv4, buf, newlen, 0, na?&na->raw:&dump.raw, &na_s);
        } else if (FD_ISSET(s->ipv6,&fset)) {
            newlen = recvfrom(s->ipv6, buf, newlen, 0, na?&na->raw:&dump.raw, &na_s);
        } else return _OI_NET_ERR;
    } 
#else
    newlen = recvfrom(s->ipv6, buf, newlen, 0, na?&na->raw:&dump.raw, &na_s);
#if defined(OI_DUALSTACK)
    _OI_SUMAP(na);
#endif
#endif
    if (newlen < 0) return _OI_NET_ERR;
    *len = newlen; 
    return 0;
}


oi_call udp_rec_any(socket_t ** res, void * buf, size_t * len, address_t * na, unsigned int ms, int num, ...) {
    socklen_t na_s = sizeof(address_t);
    address_t dump;
    int newlen = *len;
    socket_t * s;

    va_list vlst;
    fd_set fset;
    struct timeval time, *timept=0;
    _oi_sock max = 0;
    int i;

    *len = 0;
    if (res) *res = 0;
    FD_ZERO(&fset);
    if (ms) { 
        time.tv_usec = (ms%1000)*1000;
        time.tv_sec = (ms/1000);
        timept = &time;
    }
    
    va_start(vlst, num);
    for (i=0; i<num; i++) {
        s = va_arg(vlst, socket_t*);
#if defined(OI_SINGLESTACK) 
        if (s->ipv4 != _OI_SINVAL) {
            FD_SET(s->ipv4, &fset);
            if (s->ipv4 > max) max = s->ipv4;
        }
#endif
        if (s->ipv6 != _OI_SINVAL) {
            FD_SET(s->ipv6,&fset);
            if (s->ipv6 > max) max = s->ipv6;
        }
    }
    va_end(vlst);

    if (0 > select(max+1, &fset, 0, 0, timept))
        return _OI_NET_ERR;
    
    va_start(vlst, num);
    for (i=0; i<num; i++) {
        s = va_arg(vlst, socket_t*);

        if (s->ipv6 != _OI_SINVAL && FD_ISSET(s->ipv6,&fset)) {
            va_end(vlst);
            newlen = recvfrom(s->ipv6, buf, newlen, 0, na?&na->raw:&dump.raw, &na_s);
            if (newlen < 0) return _OI_NET_ERR;
            if (res) *res = s;
            *len = newlen;
            return 0;

#if defined(OI_SINGLESTACK)
        } else if (s->ipv4 != _OI_SINVAL && FD_ISSET(s->ipv4,&fset)) {
            va_end(vlst);
            
            newlen = recvfrom(s->ipv4, buf, newlen, 0, na?&na->raw:&dump.raw, &na_s);
            if (newlen < 0) return _OI_NET_ERR;
            if (res) *res = s;
            *len = newlen;
            return 0;
#endif
        }
    }
    va_end(vlst);

    return _OI_SERR_TIME;
}
