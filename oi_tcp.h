// requires -lws2_32 for windows
#ifndef OI_TCP
#define OI_TCP 1
#include "oi_os.h"
#include "oi_types.h"
#include "oi_net.h"
#include "oi_address.h"
#include "oi_socket.h"

// returns ERR_REFUSED if the reciever does not accept the connection.
// returns ERR_UNREACHABLE_HOST if the end computer is not reachable.
// returns ERR_UNREACHABLE_NET if the network containing the end computer is not reachable.
// returns ERR_TIMEOUT if the connection times out
oi_call tcp_connect(socket_t * s, address_t * a) {    
    if (a->family == AF_INET) {
#if defined(OI_IPV4) || defined(OI_SINGLESTACK)
        if (connect(s->ipv4, &a->raw, sizeof a->ipv4)) _OI_SDIE(TIME,s);
#if defined(OI_SINGLESTACK)
        if (s->ipv6 != _OI_SINVAL) {
            _OI_SCLOSE(s->ipv6);
            s->ipv6 = _OI_SINVAL;
        }
#endif
        return 0;
#elif defined(OI_DUALSTACK)
        address_t map;
        _OI_SMAP(a,map)
        if (connect(s->ipv6, &map.raw, sizeof map.ipv6)) _OI_SDIE(TIME,s);
        return 0;
#else
        return _OI_SERR_NOS;
#endif
    } else {
#if defined(OI_IPV6) || defined(OI_DUALSTACK) || defined(OI_SINGLESTACK)
        if (connect(s->ipv6, &a->raw, sizeof a->ipv6)) _OI_SDIE(TIME,s);
#if defined(OI_SINGLESTACK)
        if (s->ipv4 != _OI_SINVAL) {
            _OI_SCLOSE(s->ipv4);
            s->ipv4 = _OI_SINVAL;
        }
#endif
        return 0;
#else
        return _OI_SERR_NOS;
#endif
    }
}

// returns ERR_REFUSED if the reciever does not accept the connection.
// returns ERR_UNREACHABLE_HOST if the end computer is not reachable.
// returns ERR_UNREACHABLE_NET if the network containing the end computer is not reachable.
// returns ERR_TIMEOUT if the connection times out
oi_call tcp_timed_connect(socket_t * s, address_t * a, unsigned int ms) {
    fd_set fset;
    struct timeval time;

    FD_ZERO(&fset);
    time.tv_usec = (ms%1000)*1000;
    time.tv_sec = (ms/1000);

    if (a->family == AF_INET) {
#if defined(OI_IPV4) || defined(OI_SINGLESTACK)
        _OI_SUNBLOCK(s->ipv4);
        if (connect(s->ipv4, &a->raw, sizeof a->ipv4) && 
            _OI_NET_ERR != _OI_SERR_PROG)
                _OI_SDIE(TIME,s);
#if defined(OI_SINGLESTACK)
        if (s->ipv6 != _OI_SINVAL) {
            _OI_SCLOSE(s->ipv6);
            s->ipv6 = _OI_SINVAL;
        }
#endif
        FD_SET(s->ipv4,&fset);
        if (0 > select(s->ipv4+1, 0, &fset, 0, &time))
            _OI_SDIE(NET,s);
        
        if (FD_ISSET(s->ipv4,&fset)) {
            _OI_SBLOCK(s->ipv4);
            if (connect(s->ipv4, &a->raw, sizeof a->ipv4) && 
                _OI_NET_ERR != _OI_SERR_CONN) 
                    return _OI_TIME_ERR;
            return 0;
        } else {
            socket_destroy(s);
            return _OI_SERR_TIME;
        }
#elif defined(OI_DUALSTACK)
        address_t map;
        _OI_SMAP(a,map);
        _OI_SUNBLOCK(s->ipv6);
        if (connect(s->ipv6, &map.raw, sizeof map.ipv6) &&
            _OI_NET_ERR != _OI_SERR_PROG) 
                _OI_SDIE(TIME,s);
    
        FD_SET(s->ipv6,&fset);
        if (0 > select(s->ipv6+1, 0, &fset, 0, &time))
            _OI_SDIE(NET,s);
    
        if (FD_ISSET(s->ipv6,&fset)) {
            _OI_SBLOCK(s->ipv6);
            if (connect(s->ipv6, &map.raw, sizeof map.ipv6)) return _OI_TIME_ERR;
            return 0;
        } else {
            socket_destroy(s);
            return _OI_SERR_TIME;
        }
#else
        return _OI_SERR_NOS;
#endif
    } else {
#if defined(OI_IPV6) || defined(OI_DUALSTACK) || defined(OI_SINGLESTACK)
        _OI_SUNBLOCK(s->ipv6);
        if (connect(s->ipv6, &a->raw, sizeof a->ipv6) &&
            _OI_NET_ERR != _OI_SERR_PROG) 
                _OI_SDIE(TIME,s);
#if defined(OI_SINGLESTACK)
        if (s->ipv4 != _OI_SINVAL) {
            _OI_SCLOSE(s->ipv4);
            s->ipv4 = _OI_SINVAL;
        }
#endif
        FD_SET(s->ipv6,&fset);
        if (0 > select(s->ipv6+1, 0, &fset, 0, &time)) 
            _OI_SDIE(NET,s);
    
        if (FD_ISSET(s->ipv6,&fset)) {
            _OI_SBLOCK(s->ipv6);
            if (connect(s->ipv6, &a->raw, sizeof a->ipv6) && 
                _OI_NET_ERR != _OI_SERR_CONN) 
                    return _OI_TIME_ERR;
            return 0;
        } else {
            socket_destroy(s);
            return _OI_SERR_TIME;
        }
#else
        return _OI_SERR_NOS;
#endif
    }
}

oi_call tcp_accept(socket_t * s, socket_t * ns, address_t * na) {
    size_t na_s = sizeof(address_t);
    address_t dump;
    
#if defined(OI_SINGLESTACK)    
    ns->ipv4 = _OI_SINVAL;
    ns->ipv6 = _OI_SINVAL;
    
    if (s->ipv4 == _OI_SINVAL) {
        if (listen(s->ipv6,5)) return _OI_NET_ERR;
        ns->ipv6 = accept(s->ipv6,na?&na->raw:&dump.raw,&na_s);
        if (ns->ipv6 != _OI_SINVAL) {_OI_NET_INIT; _OI_SBLOCK(ns->ipv6);}
        return ns->ipv6 == _OI_SINVAL;
        
    } else if (s->ipv6 == _OI_SINVAL) {
        if (listen(s->ipv4,5)) return _OI_NET_ERR;
        ns->ipv4 = accept(s->ipv4,na?&na->raw:&dump.raw,&na_s);
        if (ns->ipv4 != _OI_SINVAL) {_OI_NET_INIT; _OI_SBLOCK(ns->ipv4);}
        return ns->ipv4 == _OI_SINVAL;      
        
    } else {
        fd_set fset;
        
        if (listen(s->ipv4,5)) return _OI_NET_ERR;
        if (listen(s->ipv6,5)) return _OI_NET_ERR;
        
        FD_ZERO(&fset);
        FD_SET(s->ipv4,&fset);
        FD_SET(s->ipv6,&fset);
        select((s->ipv4>s->ipv6?s->ipv4:s->ipv6)+1, &fset,0,0,0);
        
        if (FD_ISSET(s->ipv4,&fset)) {
            ns->ipv4 = accept(s->ipv4, na?&na->raw:&dump.raw, &na_s);
            if (ns->ipv4 != _OI_SINVAL) {
                _OI_NET_INIT; 
                _OI_SBLOCK(ns->ipv4);
                return 0;
            }
        } else if (FD_ISSET(s->ipv6,&fset)) {
            ns->ipv6 = accept(s->ipv6, na?&na->raw:&dump.raw, &na_s);
            if (ns->ipv6 != _OI_SINVAL) {
                _OI_NET_INIT; 
                _OI_SBLOCK(ns->ipv6);
                return 0;
            }
        }

        return _OI_NET_ERR;
    } 
#else
    ns->ipv6 = _OI_SINVAL;
    if (listen(s->ipv6,5)) return _OI_NET_ERR;

    ns->ipv6 = accept(s->ipv6,na?&na->raw:&dump.raw,&na_s);
    if (ns->ipv6 == _OI_SINVAL) return _OI_NET_ERR;

    _OI_NET_INIT; 
    _OI_SBLOCK(ns->ipv6);
#if defined(OI_DUALSTACK)
    _OI_SUMAP(na);
#endif
    return 0;
#endif
}

// returns ERR_TIMEOUT if the connection times out
oi_call tcp_timed_accept(socket_t * s, socket_t * ns, address_t * na, unsigned int ms) {
    size_t na_s = sizeof(address_t);
    address_t dump;
    fd_set fset;
    struct timeval time;
    _oi_sock max;

    FD_ZERO(&fset);
    time.tv_usec = (ms%1000)*1000;
    time.tv_sec = (ms/1000);
    
#if defined(OI_SINGLESTACK)    
    ns->ipv4 = _OI_SINVAL;
    ns->ipv6 = _OI_SINVAL;
    
    if (s->ipv4 == _OI_SINVAL) {
        if (listen(s->ipv6,5)) return _OI_NET_ERR;
        FD_SET(s->ipv6,&fset);       
        max = s->ipv6;
    } else if (s->ipv6 == _OI_SINVAL) {
        if (listen(s->ipv4,5)) return _OI_NET_ERR;
        FD_SET(s->ipv4,&fset);       
        max = s->ipv4;
    } else {
        if (listen(s->ipv4,5)) return _OI_NET_ERR;
        if (listen(s->ipv6,5)) return _OI_NET_ERR;
        FD_SET(s->ipv4,&fset);
        FD_SET(s->ipv6,&fset);
        max = s->ipv6 > s->ipv4 ? s->ipv6 : s->ipv4;
    }
#else
    ns->ipv6 = _OI_SINVAL;
    if (listen(s->ipv6,5)) return _OI_NET_ERR;

    FD_SET(s->ipv6,&fset);
    max = s->ipv6;
#endif
    
    if (0 > select(max+1, &fset, 0, 0, &time))
        return _OI_NET_ERR;
    
    if (FD_ISSET(s->ipv6,&fset)) {
        ns->ipv6 = accept(s->ipv6, na?&na->raw:&dump.raw, &na_s);
        if (ns->ipv6 != _OI_SINVAL) {
            _OI_NET_INIT; 
            _OI_SBLOCK(ns->ipv6);
#if defined(OI_DUALSTACK)
            _OI_SUMAP(na);
#endif
           return 0;
        } 
#if defined(OI_SINGLESTACK)
    } else if (FD_ISSET(s->ipv4,&fset)) {
        ns->ipv4 = accept(s->ipv4, na?&na->raw:&dump.raw, &na_s);
        if (ns->ipv4 != _OI_SINVAL) {
            _OI_NET_INIT; 
            _OI_SBLOCK(ns->ipv4);
            return 0;
        }
#endif
    } else return _OI_SERR_TIME;

    return _OI_NET_ERR;
}

// returns ERR_DISCONNECTED if the socket is disconnected.
// returns ERR_TIMEOUT if the connection times out.
oi_call tcp_send(socket_t * s, void * buf, size_t * len) {
    size_t sendlen = *len;
    int newlen;
    *len = 0;
    while(*len < sendlen) {
#if defined(OI_SINGLESTACK)
        newlen = send(s->ipv6==_OI_SINVAL?s->ipv4:s->ipv6,*len+(char*)buf,sendlen-*len,0);
#else
        newlen = send(s->ipv6,*len+(char*)buf,sendlen-*len,0);
#endif
        if (newlen < 0) return _OI_TIME_ERR;
        *len += (size_t)newlen;
    }
    return 0;
}

// returns ERR_DISCONNECTED if the socket is disconnected.
// returns ERR_TIMEOUT if the connection times out.
oi_call tcp_rec(socket_t * s, void * buf, size_t * len) {
    int newlen = *len;
    *len = 0;
#if defined(OI_SINGLESTACK)
    newlen = recv(s->ipv6==_OI_SINVAL?s->ipv4:s->ipv6,buf,newlen,0);
#else
    newlen = recv(s->ipv6,buf,newlen,0);
#endif
    if (newlen < 0) return _OI_TIME_ERR; 
    if (newlen == 0) return _OI_SERR_DISC;
    *len = newlen; 
    return 0;
}

// returns ERR_DISCONNECTED if the socket is disconnected.
// returns ERR_TIMEOUT if the connection times out.
oi_call tcp_timed_rec(socket_t * s, void * buf, size_t * len, unsigned int ms) {
    int newlen = *len;
    fd_set fset;
    struct timeval time;
    _oi_sock sock;

    *len = 0;
    FD_ZERO(&fset);
    time.tv_usec = (ms%1000)*1000;
    time.tv_sec = (ms/1000);
#if defined(OI_SINGLESTACK)
    sock = s->ipv6 == _OI_SINVAL ? s->ipv4 : s->ipv6;
#else
    sock = s->ipv6;
#endif
    FD_SET(sock,&fset);
    
    if (0 > select(sock+1, &fset, 0, 0, &time))
        return _OI_NET_ERR;
    
    if (FD_ISSET(sock,&fset)) {
        newlen = recv(sock, buf, newlen, 0);
        if (newlen < 0) return _OI_NET_ERR;
        if (newlen == 0) return _OI_SERR_DISC;
        *len = newlen; 
        return 0;
    } return _OI_SERR_TIME;
}

oi_call tcp_set_nodelay(socket_t * s, int val) {
#if defined(OI_SINGLESTACK)    
    if (s->ipv4!=_OI_SINVAL && setsockopt(s->ipv4,
                           IPPROTO_TCP,TCP_NODELAY,(char*)&val,sizeof val))
        return _OI_NET_ERR;
#endif
    if (s->ipv6!=_OI_SINVAL && setsockopt(s->ipv6,
                           IPPROTO_TCP,TCP_NODELAY,(char*)&val,sizeof val))
        return _OI_NET_ERR;
    return 0;
}

oi_call tcp_set_keepalive(socket_t * s, int val) {
#if defined(OI_SINGLESTACK)
    if (s->ipv4!=_OI_SINVAL && setsockopt(s->ipv4,
                           SOL_SOCKET,SO_KEEPALIVE,(char*)&val,sizeof val))
        return _OI_NET_ERR;
#endif
    if (s->ipv6!=_OI_SINVAL && setsockopt(s->ipv6,
                           SOL_SOCKET,SO_KEEPALIVE,(char*)&val,sizeof val))
        return _OI_NET_ERR;
    return 0;
}

oi_func int tcp_get_nodelay(socket_t * s) {
    int val = 0;
    socklen_t lval = sizeof val;
#if defined(OI_SINGLESTACK)
    getsockopt(s->ipv6==_OI_SINVAL ? s->ipv4 : s->ipv6,
                       IPPROTO_TCP,TCP_NODELAY,(char*)&val,&lval);
#else
    getsockopt(s->ipv6,IPPROTO_TCP,TCP_NODELAY,(char*)&val,&lval);
#endif
    return val;
}

oi_func int tcp_get_keepalive(socket_t * s) {
    int val = 0;
    socklen_t lval = sizeof val;
#if defined(OI_SINGLESTACK)
    getsockopt(s->ipv6==_OI_SINVAL ? s->ipv4 : s->ipv6,
                       SOL_SOCKET,SO_KEEPALIVE,(char*)&val,&lval);
#else
    getsockopt(s->ipv6,SOL_SOCKET,SO_KEEPALIVE,(char*)&val,&lval);
#endif
    return val;
}

#endif
