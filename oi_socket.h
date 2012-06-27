// for windows use -lws2_32
#ifndef OI_SOCKET
#define OI_SOCKET 1
#include "oi_os.h"
#include "oi_types.h"
#include "oi_address.h"

#ifdef OI_WIN
#define WSAGetLastError() errno;
#else
#endif

#define SOCKET_RAW SOCK_RAW
#define SOCKET_UDP SOCK_DGRAM
#define SOCKET_TCP SOCK_STREAM

#ifdef OI_WIN

typedef SOCKET socket_t;

oi_call socket_create(socket_t * s, int proto, int block) {
    WSADATA data;
    u_long blockval = block?0:1;
    int opt = 0;
    if (WSAStartup(MAKEWORD(2,2),&data)) return 1;
    if ((*s=socket(AF_INET6, proto,0))==-1) return 2;
    if (ioctlsocket(*s,FIONBIO,&blockval)) return 3;
    if (setsockopt(*s,SOL_SOCKET,IPV6_V6ONLY,&opt,sizeof(opt))) return 4;
    return 0;
}

oi_call socket_create_ipv4(socket_t * s, int proto, int block) {
    WSADATA data;
    u_long blockval = block?0:1;
    if (WSAStartup(MAKEWORD(2,2),&data)) return 1;
    if ((*s=socket(AF_INET, proto,0))==-1) return 2;
    if (ioctlsocket(*s,FIONBIO,&blockval)) return 3;
    return 0;
}

oi_call socket_create_ipv6(socket_t * s, int proto, int block) {
    WSADATA data;
    u_long blockval = block?0:1;
    int opt = 1;
    if (WSAStartup(MAKEWORD(2,2),&data)) return 1;
    if ((*s=socket(AF_INET6, proto,0))==-1) return 2;
    if (ioctlsocket(*s,FIONBIO,&blockval)) return 3;
    if (setsockopt(*s,SOL_SOCKET,IPV6_V6ONLY,&opt,sizeof(opt))) return 4;
    return 0;
}

oi_call socket_destroy(socket_t * s) {
    if(closesocket(*s)) return 1;
    if(WSACleanup()) return 2;
    return 0;
}

#else

typedef int socket_t;

oi_call socket_create(socket_t * s, int proto, int block) {
    int opt = 0;
    *s = socket(AF_INET6, proto, 0);
    if (block) fcntl(*s, F_SETFL, fcntl(*s, F_GETFL)&~O_NONBLOCK);
    if (setsockopt(*s,SOL_SOCKET,IPV6_V6ONLY,&opt,sizeof(opt))) return 4;
    return *s != -1;
}

oi_call socket_create_ipv4(socket_t * s, int proto, int block) {
    *s = socket(AF_INET, proto, 0);
    if (block) fcntl(*s, F_SETFL, fcntl(*s, F_GETFL)&~O_NONBLOCK);
    return *s != -1;
}

oi_call socket_create_ipv6(socket_t * s, int proto, int block) {
    int opt = 1;
    *s = socket(AF_INET6, proto, 0);
    if (block) fcntl(*s, F_SETFL, fcntl(*s, F_GETFL)&~O_NONBLOCK);
    if (setsockopt(*s,SOL_SOCKET,IPV6_V6ONLY,&opt,sizeof(opt))) return 4;
    return *s != -1;
}

oi_call socket_destroy(socket_t * s) {
    return close(s);
}

#endif

oi_call socket_bind(socket_t * s, address_t * a) {
    return bind(*s,(sockaddr*)a,sizeof(address_t));    
}

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


#ifdef OI_WIN
#undef errno
#endif
#endif
