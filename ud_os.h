#ifndef UD_OS_H
#define UD_OS_H

#if defined(_WIN32) || defined(__WIN32__)
	#define WIN_UD
#elif defined(linux) || defined(__linux)
	#define LINUX_UD
#elif defined(__APPLE__) || defined(MACOSX) || defined(macintosh) || defined(Macintosh)
	#define MAC_UD
#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
	#define FreeBSD_UD
#else
	#error Unknown OS
#endif
#endif

