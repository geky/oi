#ifndef OI_ERR
#define OI_ERR 1
#include "oi_os.h"
#include <netdb.h>


#ifdef OI_WIN
enum {
    ERR_IN_USE              = ERROR_BUSY        ,
    ERR_TIMEOUT             = ERROR_TIMEOUT     ,

    ERR_NOT_FOUND           = WSAHOST_NOT_FOUND ,
    ERR_NO_DATA             = WSANO_DATA        ,

    ERR_REFUSED             = WSAECONNREFUSED   ,
    ERR_UNREACHABLE_HOST    = WSAEHOSTUNREACH   ,
    ERR_UNREACHABLE_NET     = WSAENETUNREACH    ,
    ERR_DISCONNECTED        = WSAECONNRESET
};

static char _oi_err_buff[256];

oi_func const char * get_error(int err) {
    if (err < 0 ) return strerror(-err);

    FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        0, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        _oi_err_buff, sizeof _oi_err_buff, 0
    );
    return _oi_err_buff;
}

#else
#include <errno.h>
#include <netdb.h>
#include <string.h>

#ifndef _OI_EAI
#if EAI_FAMILY < 0
#   define _OI_EAI(n) (n)
#else
#   define _OI_EAI(n) (-n)
#endif
#endif

enum {
    ERR_IN_USE              = EBUSY             ,
    ERR_TIMEOUT             = ETIMEDOUT         ,

    ERR_NOT_FOUND           = EAI_NONAME        ,
    ERR_NO_DATA             = EAI_NODATA        ,

    ERR_TAKEN               = EADDRINUSE        ,
    ERR_REFUSED             = ECONNREFUSED      ,
    ERR_UNREACHABLE_HOST    = EHOSTUNREACH      ,
    ERR_UNREACHABLE_NET     = ENETUNREACH       ,
    ERR_DISCONNECTED        = ECONNRESET        
};

oi_func const char * get_error(int err) {
    if (err < 0) 
        return gai_strerror(_OI_EAI(err));
    else 
        return strerror(err);
}

#endif

#endif

