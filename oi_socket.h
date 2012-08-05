#ifndef OI_SOCKET
#define OI_SOCKET 1
#include "oi_os.h"
#include "oi_types.h"
#include "oi_net.h"
#include "oi_address.h"


#ifdef OI_WIN
#   define _OI_SERR_NOS WSAEAFNOSUPPORT
#   define _OI_SERR_TIME ERROR_TIMEOUT
#   define _OI_CON_ERR (_OI_NET_ERR==WSATIMEDOUT ? _OI_SERR_TIME : _OI_NET_ERR)
#   define _OI_SOCK SOCKET
#   define _OI_SINVAL INVALID_SOCKET
#   define _OI_SCLOSE(sock) closesocket(sock)
#   define _OI_SBLOCK(sock) {\
        u_long blk = 0; \
        if(ioctlsocket(sock,FIONBIO,&blk)) \
            _OI_SDIE(s); \
    } 
#else
#   define _OI_SERR_NOS EAFNOSUPPORT
#   define _OI_SERR_TIME ETIMEDOUT
#   define _OI_CON_ERR _OI_NET_ERR
#   define _OI_SOCK signed int
#   define _OI_SINVAL -1
#   define _OI_SCLOSE(sock) close(sock)
#   define _OI_SBLOCK(sock) \
        if (fcntl(sock, F_SETFL, fcntl(sock, F_GETFL)&~O_NONBLOCK)) \
            _OI_SDIE(s);
#endif

#if defined(OI_DUALSTACK) 
#define _OI_SOPT6(sock) { \
    int opt = 0; \
    if (setsockopt(sock,IPPROTO_IPV6,IPV6_V6ONLY,(char*)&opt,sizeof opt)) \
        _OI_SDIE(s); \
    opt = 1; \
    if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(char*)&opt,sizeof opt)) \
        _OI_SDIE(s); \
}
#else
#define _OI_SOPT4(sock) { \
    int opt = 1; \
    if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(char*)&opt,sizeof opt)) \
        _OI_SDIE(s); \
}
#if defined(IPV6_V6ONLY)
#define _OI_SOPT6(sock) { \
    int opt = 1; \
    if (setsockopt(sock,IPPROTO_IPV6,IPV6_V6ONLY,(char*)&opt,sizeof opt)) \
        _OI_SDIE(s); \
    if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(char*)&opt,sizeof opt)) \
        _OI_SDIE(s); \
}
#else
#define _OI_SOPT6(sock) _OI_SOPT4(sock)
#endif
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
#define _OI_SDIE(ss) {socket_destroy(ss); return _OI_NET_ERR;}
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
    if (s->ipv4 == _OI_SINVAL) _OI_SDIE(s);
    _OI_SOPT4(s->ipv4);
    _OI_SBLOCK(s->ipv4);
    
    if (port) {
        memset(&temp,0,sizeof temp.ipv4);
        temp.ipv4.sin_family = AF_INET;
        temp.ipv4.sin_port = htons(port);
        if (bind(s->ipv4, &temp.raw, sizeof temp.ipv4)) _OI_SDIE(s);
    }
#endif
#if defined(OI_IPV6) || defined(OI_DUALSTACK) || defined(OI_SINGLESTACK)
    s->ipv6 = socket(AF_INET6, proto, 0);
    if (s->ipv6 == _OI_SINVAL) _OI_SDIE(s);
    _OI_SOPT6(s->ipv6);
    _OI_SBLOCK(s->ipv6);

    if (port) {
        memset(&temp,0,sizeof temp.ipv6);
        temp.ipv6.sin6_family = AF_INET6;
        temp.ipv6.sin6_port = htons(port);
        if (bind(s->ipv6, &temp.raw, sizeof temp.ipv6)) _OI_SDIE(s);
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
        if (s->ipv4 == _OI_SINVAL) _OI_SDIE(s);
        _OI_SOPT4(s->ipv4);
        _OI_SBLOCK(s->ipv4);
        if (bind(s->ipv4, &a->raw, sizeof a->ipv4)) _OI_SDIE(s);
#elif defined(OI_DUALSTACK)
        _OI_NET_INIT;
        s->ipv6 = socket(AF_INET6, proto, 0);
        if (s->ipv6 == _OI_SINVAL) _OI_SDIE(s);
        _OI_SOPT6(s->ipv6);
        _OI_SBLOCK(s->ipv6);
        {   address_t map;
            _OI_SMAP(a,map) 
            if (bind(s->ipv6, &map.raw, sizeof map.ipv6)) _OI_SDIE(s);
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
        if (s->ipv6 == _OI_SINVAL) _OI_SDIE(s);
        _OI_SOPT6(s->ipv6);
        _OI_SBLOCK(s->ipv6);
        if (bind(s->ipv6, &a->raw, sizeof a->ipv6)) _OI_SDIE(s);
#else
        s->ipv6 = _OI_SINVAL;
        return _OI_SERR_NOS;
#endif
    }
    return 0;
}

oi_call socket_destroy(socket_t * s) {
    int iserr = 
#if defined(OI_SINGLESTACK)
        (s->ipv4 != _OI_SINVAL && _OI_SCLOSE(s->ipv4)) |
#endif
        (s->ipv6 != _OI_SINVAL && _OI_SCLOSE(s->ipv6));
    _OI_NET_DEINIT;
    return iserr ? _OI_NET_ERR : 0;
}

//TODO make tcp_timed_connect
oi_call tcp_connect(socket_t * s, address_t * a) {
    if (a->family == AF_INET) {
#if defined(OI_IPV4)
        if (connect(s->ipv4, &a->raw, sizeof a->ipv4)) return _OI_CON_ERR;
        return 0;
#elif defined(OI_SINGLESTACK)
        if (connect(s->ipv4, &a->raw, sizeof a->ipv4)) return _OI_CON_ERR;
        if (s->ipv6 != _OI_SINVAL) {
            _OI_SCLOSE(s->ipv6);
            s->ipv6 = _OI_SINVAL;
        }
        return 0;
#elif defined(OI_DUALSTACK)
        address_t map;
        _OI_SMAP(a,map)
        if (connect(s->ipv6, &map.raw, sizeof map.ipv6)) return _OI_CON_ERR;
        return 0;
#else
        return _OI_SERR_NOS;
#endif
    } else {
#if defined(OI_IPV6) || defined(OI_DUALSTACK)
        if (connect(s->ipv6, &a->raw, sizeof a->ipv6)) return _OI_CON_ERR;
        return 0;
#elif defined(OI_SINGLESTACK)
        if (connect(s->ipv6, &a->raw, sizeof a->ipv6)) return _OI_CON_ERR;
        if (s->ipv4 != _OI_SINVAL) {
            _OI_SCLOSE(s->ipv4);
            s->ipv4 = _OI_SINVAL;
        }
        return 0;
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
        if (0 > select((s->ipv4>s->ipv6?s->ipv4:s->ipv6)+1, &fset,0,0,0))
            return _OI_NET_ERR;    
        
        if (FD_ISSET(s->ipv4,&fset)) {
            ns->ipv4 = accept(s->ipv4, na?&na->raw:&dump.raw, &na_s);
            if (ns->ipv4 != _OI_SINVAL) {
                _OI_NET_INIT; 
                _OI_SBLOCK(ns->ipv4);
                return 0;
            } else return _OI_NET_ERR;
        } else {
            ns->ipv6 = accept(s->ipv6, na?&na->raw:&dump.raw, &na_s);
            if (ns->ipv6 != _OI_SINVAL) {
                _OI_NET_INIT; 
                _OI_SBLOCK(ns->ipv6);
                return 0;
            } else return _OI_NET_ERR;
        }
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

oi_call tcp_timed_accept(socket_t * s, socket_t * ns, address_t * na, unsigned int ms) {
    size_t na_s = sizeof(address_t);
    address_t dump;
    fd_set fset;
    struct timeval time;
    int err;
    _OI_SOCK max;

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
    
    err = select(max+1, &fset, 0, 0, &time);
    
    if (err == 0) { 
        return _OI_SERR_TIME;
    } else if (err < 0) {
        return _OI_NET_ERR;
#if defined(OI_SINGLESTACK)
    } else if (FD_ISSET(s->ipv4,&fset)) {
        ns->ipv4 = accept(s->ipv4, na?&na->raw:&dump.raw, &na_s);
        if (ns->ipv4 != _OI_SINVAL) {
            _OI_NET_INIT; 
            _OI_SBLOCK(ns->ipv4);
            return 0;
        } else return _OI_NET_ERR;
#endif
    } else {
        ns->ipv6 = accept(s->ipv6, na?&na->raw:&dump.raw, &na_s);
        if (ns->ipv6 != _OI_SINVAL) {
            _OI_NET_INIT; 
            _OI_SBLOCK(ns->ipv6);
#if defined(OI_DUALSTACK)
            _OI_SUMAP(na);
#endif
           return 0;
        } else return _OI_NET_ERR;
    }
}

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
        if (newlen < 0) return _OI_NET_ERR;
        *len += (size_t)newlen;
    }
    return 0;
}

oi_call tcp_rec(socket_t * s, void * buf, size_t * len) {
    int newlen = *len;
    *len = 0;
#if defined(OI_SINGLESTACK)
    newlen = recv(s->ipv6==_OI_SINVAL?s->ipv4:s->ipv6,buf,newlen,0);
#else
    newlen = recv(s->ipv6,buf,newlen,0);
#endif
    if (newlen < 0) return _OI_NET_ERR; 
    else {*len = newlen; return 0;}
}

oi_call tcp_timed_rec(socket_t * s, void * buf, size_t * len, unsigned int ms) {
    int newlen = *len;
    fd_set fset;
    struct timeval time;
    int err;
    _OI_SOCK sock;

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
    
    err = select(sock+1, &fset, 0, 0, &time);
    
    if (err == 0) { 
        return _OI_SERR_TIME;
    } else if (err < 0) {
        return _OI_NET_ERR;
    } else {
        newlen = recv(sock, buf, newlen, 0);
        if (newlen < 0) return _OI_NET_ERR;
        else {*len = newlen; return 0;}
    }
}

oi_call udp_send(socket_t * s, void * buf, size_t * len, address_t * a) {
    int newlen = *len;
    *len = 0;
    if (a->family == AF_INET) {
#if defined(OI_IPV4) || defined(OI_SINGLESTACK)
        newlen = sendto(s->ipv4, buf, newlen, 0, &a->raw, sizeof a->ipv4);
        if (newlen < 0) return _OI_NET_ERR;
        else {*len = newlen; return 0;}
#elif defined(OI_DUALSTACK)
        address_t map;
        _OI_SMAP(a,map)
        newlen = sendto(s->ipv6, buf, newlen, 0, &map.raw, sizeof map.ipv6);
        if (newlen < 0) return _OI_NET_ERR;
        else {*len = newlen; return 0;}
#else
        return _OI_SERR_NOS;
#endif
    } else {
#if defined(OI_IPV6) || defined(OI_DUALSTACK) || defined(OI_SINGLESTACK)
        newlen = sendto(s->ipv6, buf, newlen, 0, &a->raw, sizeof a->ipv6);
        if (newlen < 0) return _OI_NET_ERR;
        else {*len = newlen; return 0;}
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
        if (0 > select((s->ipv4>s->ipv6?s->ipv4:s->ipv6)+1, &fset,0,0,0)) 
            return _OI_NET_ERR;
        
        if (FD_ISSET(s->ipv4,&fset)) {
            newlen = recvfrom(s->ipv4, buf, newlen, 0, na?&na->raw:&dump.raw, &na_s);
        } else {
            newlen = recvfrom(s->ipv6, buf, newlen, 0, na?&na->raw:&dump.raw, &na_s);
        }
    } 
#else
    newlen = recvfrom(s->ipv6, buf, newlen, 0, na?&na->raw:&dump.raw, &na_s);
#if defined(OI_DUALSTACK)
    _OI_SUMAP(na);
#endif
#endif
    if (newlen < 0) return _OI_NET_ERR;
    else {*len = newlen; return 0;}
}

oi_call udp_timed_rec(socket_t * s, void * buf, size_t * len, address_t * na, unsigned int ms) {
    size_t na_s = sizeof(address_t);
    address_t dump;
    int newlen = *len;
    fd_set fset;
    struct timeval time;
    int err;
    _OI_SOCK max;

    *len = 0;
    FD_ZERO(&fset);
    time.tv_usec = (ms%1000)*1000;
    time.tv_sec = (ms/1000);

#if defined(OI_SINGLESTACK)    
    if (s->ipv4 == _OI_SINVAL) {
        FD_SET(s->ipv6,&fset);       
        max = s->ipv6;
    } else if (s->ipv6 == _OI_SINVAL) {
        FD_SET(s->ipv4,&fset);       
        max = s->ipv4;
    } else {
        FD_SET(s->ipv4,&fset);
        FD_SET(s->ipv6,&fset);
        max = s->ipv6 > s->ipv4 ? s->ipv6 : s->ipv4;
    }
#else
    FD_SET(s->ipv6,&fset);
    max = s->ipv6;
#endif
    
    err = select(max+1, &fset, 0, 0, &time);
    
    if (err == 0) { 
        return _OI_SERR_TIME;
    } else if (err < 0) {
        return _OI_NET_ERR;
#if defined(OI_SINGLESTACK)
    } else if (FD_ISSET(s->ipv4,&fset)) {
        newlen = recvfrom(s->ipv4, buf, newlen,0, na?&na->raw:&dump.raw, &na_s);
#endif
    } else {
        newlen = recvfrom(s->ipv6, buf, newlen,0, na?&na->raw:&dump.raw, &na_s);
    }
    
    if (newlen < 0) return _OI_NET_ERR;
    else {*len = newlen; return 0;}
}

#endif
