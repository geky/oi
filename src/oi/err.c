#include "oi/err.h"

#ifdef OI_WIN

static char _oi_err_buff[256];

const char * get_error(int err) {
    char * p;
    if (err < 0 ) return strerror(-err);
    
    FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        0, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        _oi_err_buff, sizeof _oi_err_buff-1, 0
    );
    p = strchr(_oi_err_buff,'\r');
    if (p) *p = 0;
    return _oi_err_buff;
}

#else
#include <string.h>

const char * get_error(int err) {
    if (err < 0) 
#if EAI_FAMILY < 0
        return gai_strerror(err);
#else
        return gai_strerror(-err);
#endif
    else 
        return strerror(err);
}

#endif
