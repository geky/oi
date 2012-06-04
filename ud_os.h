#ifndef UD_OS_H
#define UD_OS_H

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

