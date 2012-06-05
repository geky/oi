#ifndef OI_OS
#define OI_OS

#if defined(_WIN32) || defined(__WIN32__)
	#define OI_WIN
        #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
        #endif
#elif defined(linux) || defined(__linux)
	#define OI_LINUX
#elif defined(__APPLE__) || defined(MACOSX) || defined(macintosh) || defined(Macintosh)
	#define OI_MAC
#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
	#define OI_FreeBSD
#else
	#error oi does not recognize OS
#endif
#endif

