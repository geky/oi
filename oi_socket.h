#ifndef OI_SOCKET
#define OI_SOCKET
#include "oi_os.h"
#include "oi_types.h"

#ifdef OI_WIN
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif

#define SOCKET_UDP SOCK_DGRAM
#define SOCKET_TCP SOCK_STREAM
#define SOCKET_IP  SOCK_RAW

typedef sockaddr_in address_t;

static inline int address_create(address_t * a, uint32 ip, uint16 port) {
    memset(a->sin_zero,0,8);
    a->sin_family = AF_INET;
    a->sin_addr.s_addr = htonl(ip);
    a->sin_port = htons(port);
    return 0;
}

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
    return s != -1;
}

static inline int socket_destroy(socket_t * s) {
    return close(s);
}

#endif

static inline int socket_bind(socket_t * s, address_t * a) {
	return bind(*s,a,sizeof a);    
}

static inline int socket_conect(socket_t * s, address_t * a) {
	return connect(*s,a,sizeof a);
}

static inline int socket_send(socket_t * s, void * buf, size_t len) {
    return send(*s,buf,len,0) == -1;
}

static inline int socket_send_to(socket_t * s, void * buf, size_t len, address_t * a) {
    return sendto(*s,buf,len,0,a,sizeof address_t) == -1;
}

static inline int socket_rec(socket_t * s, void * buf, size_t len, address_t * a, size_t * inlen) {
    ssize_t rsize = recv(*s,buf,len);
    if (rsize >= 0) *inlen = rsize;
    return rsize < 0;
}

static inline int socket_rec_from(socket_t * s, void * buf, size_t len, address_t * a, size_t * inlen) {
    ssize_t rsize = recvfrom(*s,buf,len,a,sizeof address_t);
    if (rsize >= 0) *inlen = rsize;
    return rsize < 0;
}


//#endif

#endif
