//you may define
// OI_IPV4_ONLY
// OI_IPV6_ONLY
// OI_DUALSTACK
// OI_SINGLESTACK
#ifndef OI_NET
#define OI_NET 1

#include "oi_os.h"
#include <string.h>

#ifdef OI_MSVC
#   pragma comment(lib,"ws2_32.lib")
#endif

#ifdef OI_WIN
#   include <winsock2.h>
#   include <ws2tcpip.h>
#
#   if WINVER >= 0x0600 && !defined(IPV6_V6ONLY)
#       define IPV6_V6ONLY 27
#   endif
#
#   //yes, the return in the macro is intentional
#   define _OI_NET_INIT {                       \
        WSADATA wsadata;                        \
        if(WSAStartup(MAKEWORD(2,2),&wsadata))  \
            return 6; }

#   define _OI_NET_DEINIT                       \
        if (WSACleanup()) return 5;

#   define _OI_NET_ERR WSAGetLastError()
#
#else
#
#   include <unistd.h>
#   include <netinet/in.h>
#   include <arpa/inet.h>
#   include <sys/types.h>
#   include <sys/socket.h>
#   include <netdb.h>
#   include <fcntl.h>
#
#   define _OI_NET_INIT
#   define _OI_NET_DEINIT
#   define _OI_NET_ERR errno
#
#endif

#if !defined(OI_SINGLESTACK) && !defined(OI_DUALSTACK) && !defined(OI_IPV4) && !defined(OI_IPV6)
#ifdef IPV6_V6ONLY
#   define OI_DUALSTACK
#else
#   define OI_SINGLESTACK
#endif
#endif

#endif
