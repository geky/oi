#ifndef OI_OS
#define OI_OS 1

#if defined(_WIN32) || defined(__WIN32__)
#   define OI_WIN
#   ifdef _MSC_VER
#   define OI_VISUAL_STUDIO
#   endif
#   ifndef WIN32_LEAN_AND_MEAN
#   define WIN32_LEAN_AND_MEAN
#   endif
#   include <windows.h>
#elif defined(linux) || defined(__linux)
#   define OI_LINUX
#   ifndef _XOPEN_SOURCE
#   define _XOPEN_SOURCE 500
#   endif
#elif defined(__APPLE__) || defined(MACOSX) || defined(macintosh) || defined(Macintosh)
#   define OI_MAC
#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
#   define OI_FreeBSD
#else
#   warning oi does not recognize OS
#   define OI_UNKNOWN_OS
#endif
#endif
