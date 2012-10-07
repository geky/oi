#ifndef OI_NET
#define OI_NET 1
#include "oi/os.h"


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
#else
#   include <netdb.h>
#
#endif

// you may define
//    OI_IPV4_ONLY
//    OI_IPV6_ONLY
//    OI_DUALSTACK
//    OI_SINGLESTACK
// to control IP version
#if !defined(OI_SINGLESTACK) && !defined(OI_DUALSTACK) && !defined(OI_IPV4) && !defined(OI_IPV6)
#ifdef IPV6_V6ONLY
#   define OI_DUALSTACK
#else
#   define OI_SINGLESTACK
#endif
#endif


#endif
