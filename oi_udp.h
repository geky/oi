// requires -lws2_32 for windows
#ifndef OI_UDP
#define OI_UDP 1
#include "oi_os.h"
#include "oi_types.h"
#include "oi_net.h"
#include "oi_address.h"
#include "oi_socket.h"

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
    size_t na_s = sizeof(address_t);
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

// returns ERR_TIMEOUT on timeout
oi_call udp_select_rec(socket_t * dst, void * buf, size_t * len, address_t * na, unsigned int ms, int num, ...) {
    size_t na_s = sizeof(address_t);
    address_t dump;
    int newlen = *len;
    va_list vlst;
    fd_set fset;
    struct timeval time;
    _oi_sock max = 0;

    *len = 0;
    va_start(vlst,&num);
    FD_ZERO(&fset);
    time.tv_usec = (ms%1000)*1000;
    time.tv_sec = (ms/1000);

    while (num--) {
#if defined(OI_SINGLESTACK) 
        socket_t * next = va_arg(vlst, socket_t*);
        
        if (next->ipv4 != _OI_SINVAL) {
            FD_SET(s->ipv4, &fset);
            if (next->ipv4 > max) max = next->ipv4;
        }
#endif
        if (next->ipv6 != _OI_SINVAL) {
            FD_SET(s->ipv6,&fset);
            if (next->ipv6 > max) max = next->ipv6;
        }
    }
    
    if (0 > select(max+1, &fset, 0, 0, &time))
        return _OI_NET_ERR;
        //TODO below heres
    if (FD_ISSET(s->ipv6,&fset)) {
        newlen = recvfrom(s->ipv6, buf, newlen,0, na?&na->raw:&dump.raw, &na_s);
#if defined(OI_SINGLESTACK)
    } else if (FD_ISSET(s->ipv4,&fset)) {
        newlen = recvfrom(s->ipv4, buf, newlen,0, na?&na->raw:&dump.raw, &na_s);
#endif
    } else return _OI_SERR_TIME;
    
    if (newlen < 0) return _OI_NET_ERR;
    *len = newlen; 
    return 0;
}

#endif
