#ifndef OI_NET
#define OI_NET 1
#include "oi/os.h"


// oi_net determines your system's network environment and sets up 
// related macros for oi to use internally.
//
// For the most part oi tries to hide the IPv4 and IPv6  differences, 
// but if you need a specific one you can define OI_IPV4 or OI_IPV6 
// before oi_net's inclussion to restrict network usage to a single version.


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


// Definition of IP version usage //
#if !defined(OI_SINGLESTACK) && !defined(OI_DUALSTACK) && !defined(OI_IPV4) && !defined(OI_IPV6)
#ifdef IPV6_V6ONLY
#   define OI_DUALSTACK
#else
#   define OI_SINGLESTACK
#endif
#endif
////////////////////////////////////


#endif
