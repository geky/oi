#ifndef OI_OS
#define OI_OS 1

#if defined(__cplusplus) 
#   define OI_CPP
#else
#   define OI_C
#endif

#if defined(_MSC_VER)
#   define OI_MSVC
#elif defined(__clang__)
#   define OI_CLANG
#elif defined(__GNUC__)
#   define OI_GCC
#else
#   warning oi does not recognize compiler
#   define OI_UNKNOWN_CC
#endif

#if defined(_WIN32) || defined(__WIN32__)
#   define OI_WIN
#
#   ifndef WIN32_LEAN_AND_MEAN
#   define WIN32_LEAN_AND_MEAN
#   endif
#
#   ifndef _WIN32_WINNT
#   define _WIN32_WINNT 0x0501
#   endif
#
#   include <windows.h>
#
#elif defined(linux) || defined(__linux)
#   define OI_LINUX
#
#   ifndef _XOPEN_SOURCE
#   define _XOPEN_SOURCE 500
#   endif
#
#   ifndef __USE_UNIX98
#   define __USE_UNIX98
#   endif
#
#elif defined(__APPLE__) || defined(MACOSX) || defined(macintosh) || defined(Macintosh)
#   define OI_MAC
#
#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
#   define OI_BSD
#
#else
#   warning oi does not recognize OS
#   define OI_UNKNOWN_OS
#endif

#ifdef OI_BSD
#   define _OI_NE(n) (-n)
#else
#   define _OI_NE(n) (n)
#endif

#ifndef OI_MSVC
#   define oi_func static inline
#else
#   define oi_func static __inline
#endif

#define oi_call oi_func int

#endif
