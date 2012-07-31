#ifndef OI_ERR
#define OI_ERR 1
#include "oi_os.h"

enum {
#ifdef OI_WIN
    ERR_SUCCESS = 0,
    ERR_FAILURE = 1,
#else
    ERR_SUCCESS         = 0                 ,
    ERR_FAILURE         = 1                 ,
    ERR_IN_USE          = EBUSY             ,
    ERR_TIMEOUT         = ETIMEDOUT         ,
    ERR_INVALID         = EINVAL            ,
    ERR_DEADLOCK        = EDEADLK           ,
    ERR_NO_PERMISSION   = EPERM             ,
    ERR_NO_MEMORY       = ENOMEM            ,
    ERR_NO_RESOURCE     = EAGAIN            ,
    ERR_INVALID_THREAD  = ESRCH             ,

                        = EAI_ADDRFAMILY    ,
    ERR_NO_RESOURCE     = EAI_AGAIN         ,
                        = EAI_BADFLAGS      ,
                        = EAI_FAIL          ,
                        = EAI_FAMILY        ,
                        = EAI_MEMORY        ,
                        = EAI_NODATA        ,
                        = EAI_NONAME        ,
                        = EAI_SERVICE       ,
                        = EAI_SOCKTYPE      ,
                        = EAI_SYSTEM        ,

                        = EAFNOSUPPORT      ,
                        = EMFILE            ,
                        = ENFILE            ,
                        = EPROTONOSUPPORT   ,
                        = EPROTOTYPE        ,
                        = EACCES            ,
                        = ENOBUFS           ,

                        = EADDRINUSE        ,
                        = EADDRNOTAVAIL     ,
                        = EAFNOSUPPORT      ,
                        = EBADF             ,
                        = ENOTSOCK          ,
                        = EOPNOTSUPP        ,
                        = EDESTADDRREQ      ,
                        = EIO               ,
                        = ELOOP             ,
                        = ENAMETOOLONG      ,
                        = ENOENT            ,
                        = ENOTDIR           ,
                        = EROFS             ,
                        = EISCONN           ,
                        = ENAMETOOLONG      ,
                        = ENOBUFS           ,
                        
#endif
}

#endif

