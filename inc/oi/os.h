#ifndef OI_OS
#define OI_OS 1


// oi_os is the starting point for OI. 
// It determines your compiling enviroment and sets up useful macros. 
// It may be necessary for oi_os to be included before other files 
// so oi_os can set up correctly.
//
// OI will probably work on other configurations, but they have 
// not yet been tested; If you use oi on an untested enviroment and 
// can confirm that it works, please add the definitions below and add
// your changes to the github repo.


// Definition of Language //
#if defined(__cplusplus) 
#   define OI_CPP
#else
#   define OI_C
#endif
////////////////////////////

// Definition of Compiler //
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
////////////////////////////

// Definition of Operating System //
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
#   ifndef _GNU_SOURCE
#   define _GNU_SOURCE 1
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
////////////////////////////////////

// Definition of oi_call for functions //
#define oi_call int
/////////////////////////////////////////

#endif
