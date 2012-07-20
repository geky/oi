#ifndef OI_NET
#define OI_NET 1

#include "oi_os.h"

#ifdef OI_WIN
#   include <winsock2.h>
#   include <WS2tcpip.h>
#
#   //yes, the return in the macro is intentional
#   define _OI_NET_INIT {                       \
        WSADATA wsadata;                        \
        if(WSAStartup(MAKEWORD(2,2),&wsadata))  \
            return 6; }

#   define _OI_NET_DEINIT                       \
        if (WSACleanup()) return 5;

#else
#   define _OI_NET_INIT
#   define _OI_NET_DEINIT
#
#endif

#ifdef IPV6_V6ONLY
#   define OI_DUALSTACK
#endif

#endif
