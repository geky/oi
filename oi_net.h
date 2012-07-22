#ifndef OI_NET
#define OI_NET 1

#include "oi_os.h"

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

#ifdef IPV6_V6ONLY
#   define _OI_NET_DS
#endif

#endif
