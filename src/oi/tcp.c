#include "oi/tcp.h"


oi_call tcp_connect(socket_t * s, address_t * a) {    
    if (a->family == AF_INET) {
#if defined(OI_IPV4) || defined(OI_SINGLESTACK)
        if (connect(s->ipv4, &a->raw, sizeof a->ipv4)) 
            _OI_SDIE(_OI_TIME_ERR,s);
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
        if (connect(s->ipv6, &map.raw, sizeof map.ipv6)) 
            _OI_SDIE(_OI_TIME_ERR,s);
        return 0;
#else
        return _OI_SERR_NOS;
#endif
    } else {
#if defined(OI_IPV6) || defined(OI_DUALSTACK) || defined(OI_SINGLESTACK)
        if (connect(s->ipv6, &a->raw, sizeof a->ipv6)) 
            _OI_SDIE(_OI_TIME_ERR,s);
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


oi_call tcp_connect_any(socket_t ** res_s, address_t ** res_a, unsigned int ms, int num, ...) {
    fd_set fset;
    struct timeval time, *timept=0;
    socket_t * s;
    address_t * a;
    va_list vlst;
    _oi_sock max = 0;
    int i, err = 0;

    if (res_s) *res_s = 0;
    if (res_a) *res_a = 0;
    FD_ZERO(&fset);
    if (ms) {
        time.tv_usec = (ms%1000)*1000;
        time.tv_sec = (ms/1000);
        timept = &time;
    }
    
    va_start(vlst, num);
    for (i=0; i<num; i++) {
        s = va_arg(vlst, socket_t*);
        a = va_arg(vlst, address_t*);
        
        if (a->family == AF_INET) {
#if defined(OI_IPV4) || defined(OI_SINGLESTACK)
#if defined(OI_SINGLESTACK)
            if (s->ipv6 != _OI_SINVAL) {
                _OI_SCLOSE(s->ipv6);
                s->ipv6 = _OI_SINVAL;
            }
#endif
            _OI_SUNBLOCK(s->ipv4);
            
            if (connect(s->ipv4, &a->raw, sizeof a->ipv4) && 
                  _OI_NET_ERR != _OI_SERR_PROG) {
                _OI_SCLOSE(s->ipv4);
                s->ipv4 = _OI_SINVAL;
            } else {
                FD_SET(s->ipv4, &fset);
                if (s->ipv4 > max) max = s->ipv4;
            }
#elif defined(OI_DUALSTACK)
            address_t map;
            _OI_SMAP(a, map);
            _OI_SUNBLOCK(s->ipv6);
            
            if (connect(s->ipv6, &map.raw, sizeof map.ipv6) && 
                  _OI_NET_ERR != _OI_SERR_PROG) {
                _OI_SCLOSE(s->ipv6);
                s->ipv6 = _OI_SINVAL;
            } else {
                FD_SET(s->ipv6, &fset);
                if (s->ipv6 > max) max = s->ipv6;
            }
#else
            _OI_SCLOSE(s->ipv6);
            s->ipv6 = _OI_SINVAL;
#endif
        } else {
#if defined(OI_IPV6) || defined(OI_DUALSTACK) || defined(OI_SINGLESTACK)
#if defined(OI_SINGLESTACK)
            if (s->ipv4 != _OI_SINVAL) {
                _OI_SCLOSE(s->ipv4);
                s->ipv4 = _OI_SINVAL;
            }
#endif
            _OI_SUNBLOCK(s->ipv6);
            
            if (connect(s->ipv6, &a->raw, sizeof a->ipv6) &&
                  _OI_NET_ERR != _OI_SERR_PROG) {
                _OI_SCLOSE(s->ipv6);
                s->ipv6 = _OI_SINVAL;
            } else {
                FD_SET(s->ipv6, &fset);
                if (s->ipv6 > max) max = s->ipv6;
            }
#else
            _OI_SCLOSE(s->ipv4);
            s->ipv4 = _OI_SINVAL;
#endif
        }
    }
    va_end(vlst);
    
    
    i = select(max+1, 0, &fset, 0, timept);
    
    if (i == 0)
        err = _OI_SERR_TIME;
    else if (i < 0)
        err = _OI_NET_ERR;
    
    va_start(vlst, num);
    for (i=0; i<num; i++) {
        s = va_arg(vlst, socket_t*);
        a = va_arg(vlst, address_t*);
        
        if (a->family == AF_INET) {
#if defined(OI_IPV4) || defined(OI_SINGLESTACK)
            if (s->ipv4 != _OI_SINVAL) {
                if (FD_ISSET(s->ipv4, &fset)) {
                    _OI_SBLOCK(s->ipv4);
                    if (connect(s->ipv4, &a->raw, sizeof a->ipv4) &&
                          _OI_NET_ERR != _OI_SERR_CONN) {
                        err = _OI_TIME_ERR;
                        _OI_SCLOSE(s->ipv4);
                        s->ipv4 = _OI_SINVAL;
                    } else {
                        if (res_s) *res_s = s;
                        if (res_a) *res_a = a;
                    }
                } else {
                    _OI_SCLOSE(s->ipv4);
                    s->ipv4 = _OI_SINVAL;
                }
            }
#elif defined(OI_DUALSTACK)
            address_t map;
            _OI_SMAP(a, map);
            
            if (s->ipv6 != _OI_SINVAL) {
                if (FD_ISSET(s->ipv6, &fset)) {
                    _OI_SBLOCK(s->ipv6);
                    if (connect(s->ipv6, &map.raw, sizeof map.ipv6) &&
                          _OI_NET_ERR != _OI_SERR_CONN) {
                        err = _OI_TIME_ERR;
                        _OI_SCLOSE(s->ipv6);
                        s->ipv6 = _OI_SINVAL;
                    } else {
                        if (res_s) *res_s = s;
                        if (res_a) *res_a = a;
                    }
                } else {
                    _OI_SCLOSE(s->ipv6);
                    s->ipv6 = _OI_SINVAL;
                }
            }
#endif
        } else {
#if defined(OI_IPV6) || defined(OI_DUALSTACK) || defined(OI_SINGLESTACK)
            if (s->ipv6 != _OI_SINVAL) {
                if (FD_ISSET(s->ipv6, &fset)) {
                    _OI_SBLOCK(s->ipv6);
                    if (connect(s->ipv6, &a->raw, sizeof a->ipv6) &&
                          _OI_NET_ERR != _OI_SERR_CONN) {
                        err = _OI_TIME_ERR;
                        _OI_SCLOSE(s->ipv6);
                        s->ipv6 = _OI_SINVAL;
                    } else {
                        if (res_s) *res_s = s;
                        if (res_a) *res_a = a;
                    }
                } else {
                    _OI_SCLOSE(s->ipv6);
                    s->ipv6 = _OI_SINVAL;
                }
            }
#endif
        }
    }
    va_end(vlst);
    
    return err;
}


oi_call tcp_accept(socket_t * s, socket_t * ns, address_t * na) {
    socklen_t na_s = sizeof(address_t);
    address_t dump;
    
#if defined(OI_SINGLESTACK)
    fd_set fset;

    FD_ZERO(&fset);
    ns->ipv4 = _OI_SINVAL;
    ns->ipv6 = _OI_SINVAL;

    if (s->ipv4 != _OI_SINVAL) {
        if (listen(s->ipv4,5)) return _OI_NET_ERR;
        FD_SET(s->ipv4, &fset);
    }
    if (s->ipv6 != _OI_SINVAL) {
        if (listen(s->ipv6,5)) return _OI_NET_ERR;
        FD_SET(s->ipv6, &fset);
    }
    
    select((s->ipv4>s->ipv6?s->ipv4:s->ipv6)+1, &fset,0,0,0);
    
    if (s->ipv4 != _OI_SINVAL && FD_ISSET(s->ipv4,&fset)) {
        ns->ipv4 = accept(s->ipv4, na?&na->raw:&dump.raw, &na_s);
        if (ns->ipv4 != _OI_SINVAL) {
            _OI_NET_INIT; 
            _OI_SBLOCK(ns->ipv4);
            return 0;
        }
    } else if (s->ipv6 != _OI_SINVAL && FD_ISSET(s->ipv6,&fset)) {
        ns->ipv6 = accept(s->ipv6, na?&na->raw:&dump.raw, &na_s);
        if (ns->ipv6 != _OI_SINVAL) {
            _OI_NET_INIT; 
            _OI_SBLOCK(ns->ipv6);
            return 0;
        }
    }

    return _OI_NET_ERR;
#else

    ns->ipv6 = _OI_SINVAL;
    if (listen(s->ipv6,5)) return _OI_NET_ERR;

    ns->ipv6 = accept(s->ipv6, na?&na->raw:&dump.raw, &na_s);
    if (ns->ipv6 == _OI_SINVAL) return _OI_NET_ERR;

    _OI_NET_INIT;
    _OI_SBLOCK(ns->ipv6);
#if defined(OI_DUALSTACK)
    _OI_SUMAP(na);
#endif
    return 0;
#endif
}


oi_call tcp_accept_any(socket_t ** res_s, socket_t * ns, address_t * na, unsigned int ms, int num, ...) {
    socklen_t na_s = sizeof(address_t);
    address_t dump;
    fd_set fset;
    va_list vlst;
    struct timeval time, *timept=0;
    socket_t * s;
    _oi_sock max = 0;
    int i;

    if (res_s) *res_s = 0;
    FD_ZERO(&fset);
    if (ms) {
        time.tv_usec = (ms%1000)*1000;
        time.tv_sec = (ms/1000);
        timept = &time;
    }

#if defined(OI_SINGLESTACK)
    ns->ipv4 = _OI_SINVAL;
#endif
    ns->ipv6 = _OI_SINVAL;

    va_start(vlst, num);
    for (i=0; i<num; i++) {
        s = va_arg(vlst, socket_t*);

#if defined(OI_SINGLESTACK)     
        if (s->ipv4 != _OI_SINVAL) {
            listen(s->ipv4,5);
            FD_SET(s->ipv4,&fset);       
            if (s->ipv4 > max) max = s->ipv4;
        }
#endif
        if (s->ipv6 != _OI_SINVAL) {
            listen(s->ipv6,5);
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

        if (s->ipv6 != _OI_SINVAL && FD_ISSET(s->ipv6, &fset)) {
            va_end(vlst);

            ns->ipv6 = accept(s->ipv6, na?&na->raw:&dump.raw, &na_s);
            if (ns->ipv6 == _OI_SINVAL) return _OI_NET_ERR;
            if (res_s) *res_s = s;
            _OI_NET_INIT; 
            _OI_SBLOCK(ns->ipv6);
#if defined(OI_DUALSTACK)
            _OI_SUMAP(na);
#endif
            return 0;

#if defined(OI_SINGLESTACK)
        } else if (s->ipv4 != _OI_SINVAL && FD_ISSET(s->ipv4, &fset)) {
            va_end(vlst);

            ns->ipv4 = accept(s->ipv4, na?&na->raw:&dump.raw, &na_s);
            if (ns->ipv4 == _OI_SINVAL) return _OI_NET_ERR;
            if (res_s) *res_s = s;
            _OI_NET_INIT; 
            _OI_SBLOCK(ns->ipv4);
            return 0;
#endif
        }
    }
    va_end(vlst);

    return _OI_SERR_TIME;
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
        if (newlen < 0) return _OI_TIME_ERR;
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
    if (newlen < 0) return _OI_TIME_ERR; 
    if (newlen == 0) return _OI_SERR_DISC;
    *len = newlen; 
    return 0;
}


oi_call tcp_rec_any(socket_t ** res_s, void * buf, size_t * len, unsigned int ms, int num, ...) {
    int newlen = *len;
    socket_t * s;
    fd_set fset;
    va_list vlst;
    struct timeval time, *timept=0;
    _oi_sock max = 0;
    int i;

    *len = 0;
    if (res_s) *res_s = 0;
    FD_ZERO(&fset);
    if (ms) {
        time.tv_usec = (ms%1000)*1000;
        time.tv_sec = (ms/1000);
        timept = &time;
    }

    va_start(vlst, num);
    for (i=0; i<num; i++) {
        s = va_arg(vlst, socket_t*);

        if (s->ipv6 != _OI_SINVAL) {
            FD_SET(s->ipv6, &fset);
            if (s->ipv6 > max) max = s->ipv6;
#if defined(OI_SINGLESTACK)
        } else if (s->ipv4 != _OI_SINVAL) {
            FD_SET(s->ipv4, &fset);
            if (s->ipv4 > max) max = s->ipv4;
#endif
        }
    }
    va_end(vlst);
    
    if (0 > select(max+1, &fset, 0, 0, &time))
        return _OI_NET_ERR;

    va_start(vlst, num);
    for (i=0; i<num; i++) {
        s = va_arg(vlst, socket_t*);

        if (s->ipv6 != _OI_SINVAL && FD_ISSET(s->ipv6, &fset)) {
            va_end(vlst);

            if (res_s) *res_s = s;
            newlen = recv(s->ipv6, buf, newlen, 0);
            if (newlen < 0) return _OI_NET_ERR;
            if (newlen == 0) return _OI_SERR_DISC;
            *len = newlen;
            return 0;

#if defined(OI_SINGLESTACK)
        } else if (s->ipv4 != _OI_SINVAL && FD_ISSET(s->ipv4, &fset)) {
            va_end(vlst);

            if (res_s) *res_s = s;
            newlen = recv(s->ipv6, buf, newlen, 0);
            if (newlen < 0) return _OI_NET_ERR;
            if (newlen == 0) return _OI_SERR_DISC;
            *len = newlen; 
            return 0;
#endif
        }
    }
    va_end(vlst);

    return _OI_SERR_TIME;
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


int tcp_get_nodelay(socket_t * s) {
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


int tcp_get_keepalive(socket_t * s) {
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

