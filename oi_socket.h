#ifndef OI_SOCKET
#define OI_SOCKET
#include "oi_os.h"
#include "oi_types.h"

#ifdef OI_WIN
#include <winsock2.h>
#define WSAGetLastError() errno;
#else
#include <sys/socket.h>
#include <string.h>
#include <fcntl.h>
#endif

typedef struct sockaddr address_t;

static inline int address_create(address_t * a, uint32 ip, uint16 port) {
	a->sa_family = AF_INET;
	*(uint16*)(a->sa_data)  = htons(port);
	*(uint32*)(a->sa_data+2)= htonl(ip);
    memset(a->sa_data+6,0,8);
    return 0;
}


#define SOCKET_UDP SOCK_DGRAM
#define SOCKET_TCP SOCK_STREAM

#ifdef OI_WIN

typedef SOCKET socket_t;

static inline int socket_create(socket_t * s, int proto, int block) {
	WSADATA data;
	u_long blockval = block?0:1;
	if (WSAStartup(MAKEWORD(2,2),&data)) return 1;
    if ((*s=socket(AF_INET, proto,0))==-1) return 2;
	if (ioctlsocket(s,FIONBIO,&blockval)) return 3;
    return 0;
}

static inline int socket_destroy(socket_t * s) {
    if(closesocket(s)) return 1;
	if(WSACleanup()) return 2;
	return 0;
}

#else

typedef int socket_t;

static inline int socket_create(socket_t * s, int proto, int block) {
    *s = socket(AF_INET, proto, 0);
    if (block) fcntl(*s, F_SETFL, fcntl(*s, F_GETFL)&~O_NONBLOCK);
    return *s != -1;
}

static inline int socket_destroy(socket_t * s) {
    return close(s);
}

#endif

static inline int socket_bind(socket_t * s, address_t * a) {
	return bind(*s,a,sizeof(address_t));    
}

static inline int tcp_connect(socket_t * s, address_t * a) {
	return connect(*s,a,sizeof(address_t));
}

static inline int tcp_accept(socket_t * s, socket_t * ns, address_t * na) {
	int na_s = sizeof(address_t);
	if (listen(*s,5)) return 2;
	*ns = accept(*s,na,&na_s);
	return (*ns == -1);
}

static inline int tcp_send(socket_t * s, void * buf, size_t len) {
    return send(*s,buf,len,0) == -1;
}

static inline int tcp_rec(socket_t * s, void * buf, size_t len, size_t * inlen) {
    ssize_t rsize = recv(*s,buf,len,0);
    if (rsize >= 0) *inlen = rsize;
    return rsize < 0;
}

static inline int udp_send(socket_t * s, void * buf, size_t len, address_t * a) {
    return sendto(*s,buf,len,0,a,sizeof(address_t)) == -1;
}

static inline int udp_rec(socket_t * s, void * buf, size_t len, size_t * inlen, address_t * a) {
	int a_s = sizeof(address_t);
    ssize_t rsize = recvfrom(*s,buf,len,0,a,&a_s);
    if (rsize >= 0 && inlen) *inlen = rsize;
    return rsize < 0;
}


#ifdef OI_WIN
#undef errno
#endif
#endif
