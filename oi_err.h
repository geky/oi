#ifndef OI_ERR
#define OI_ERR 1
#include "oi_os.h"
#include <netdb.h>

#ifdef OI_WIN
enum {
    ERR_SUCCESS     = 0                 ,
    
    ERR_IN_USE      = ERROR_BUSY        ,
    ERR_TIMEOUT     = ERROR_TIMEOUT     ,  
}



oi_func const char * get_error(int err) {
    
}

#else
#include <errno.h>
#include <netdb.h>
#include <string.h>

enum {
    ERR_SUCCESS     = 0                 ,

    ERR_IN_USE      = EBUSY             ,
    ERR_TIMEOUT     = ETIMEDOUT         ,
}

oi_func const char * get_error(int err) {
    if (err < 0) 
        return gai_strerror(_OI_NE(err));
    else 
        return strerror(err);
}

#endif

#endif

