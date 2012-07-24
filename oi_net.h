//you may define
// OI_IPV4_ONLY
// OI_IPV6_ONLY
// OI_DUALSTACK
// OI_SINGLESTACK
#ifndef OI_NET
#define OI_NET 1

#include "oi_os.h"

#define UDP SOCK_DGRAM
#define TCP SOCK_STREAM
#define IPV4 AF_INET
#define IPV6 AF_INET6

#ifdef OI_MSVC
#   pragma comment(lib,"ws2_32.lib")
#endif

#ifdef OI_WIN
#   include <winsock2.h>
#   include <ws2tcpip.h>
#
#   if _WIN32_WINNT >= 0x0600 && !defined(IPV6_V6ONLY)
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
#   define _OI_NET_INIT
#   define _OI_NET_DEINIT
#   define _OI_NET_ERR errno
#
#endif

#if !defined(OI_SINGLESTACK) && !defined(OI_DUALSTACK)
#ifdef IPV6_V6ONLY
#   define OI_DUALSTACK
#else
#   define OI_SINGLESTACK
#endif
#endif

#endif
