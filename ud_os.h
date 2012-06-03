#if !(defined(UD_WIN) || defined(UD_LINUX) || defined(UD_MAC) || defined(UD_FreeBSD))

#if defined(_WIN32) || defined(__WIN32__)
	#define UD_WIN
#elif defined(linux) || defined(__linux)
	#define UD_LINUX
#elif defined(__APPLE__) || defined(MACOSX) || defined(macintosh) || defined(Macintosh)
	#define UD_MAC
#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
	#define UD_FreeBSD
#else
	#error Unknown OS
#endif
#endif

