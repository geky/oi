#ifndef OI_ERR
#define OI_ERR 1

#include "oi/os.h"

#ifdef OI_CPP
extern "C" {
#endif


// Reliable error values
#ifdef OI_WIN
enum {
    ERR_IN_USE              = ERROR_BUSY        ,
    ERR_TIMEOUT             = ERROR_TIMEOUT     ,
    ERR_NOT_FOUND           = WSAHOST_NOT_FOUND ,
    ERR_NO_DATA             = WSANO_DATA        ,
    ERR_TAKEN               = WSAEADDRINUSE     ,
    ERR_REFUSED             = WSAECONNREFUSED   ,
    ERR_UNREACHABLE_HOST    = WSAEHOSTUNREACH   ,
    ERR_UNREACHABLE_NET     = WSAENETUNREACH    ,
    ERR_DISCONNECTED        = WSAECONNRESET
};
#else
#include <errno.h>
#include <netdb.h>
enum {
    ERR_IN_USE              = EBUSY             ,
    ERR_TIMEOUT             = ETIMEDOUT         ,
#if EAI_FAMILY < 0
    ERR_NOT_FOUND           = EAI_NONAME        ,
    ERR_NO_DATA             = EAI_NODATA        ,
#else
    ERR_NOT_FOUND           = -EAI_NONAME       ,
    ERR_NO_DATA             = -EAI_NODATA       ,
#endif
    ERR_TAKEN               = EADDRINUSE        ,
    ERR_REFUSED             = ECONNREFUSED      ,
    ERR_UNREACHABLE_HOST    = EHOSTUNREACH      ,
    ERR_UNREACHABLE_NET     = ENETUNREACH       ,
    ERR_DISCONNECTED        = ECONNRESET        
};
#endif


const char * get_error(int err);


#ifdef OI_CPP
}
#endif

#endif
