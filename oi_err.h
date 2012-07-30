#ifndef OI_ERR
#define OI_ERR 1
#include "oi_os.h"

#ifdef OI_WIN
#   define ERR_SUCCESS 0
#   define ERR_FAILURE 1
#else
#   define ERR_SUCCESS 0
#   define ERR_FAILURE 1
#   define ERR_TIMEOUT ETIMEDOUT
#   define ERR_INVALID EINVAL
#   define //EPERM??
#   define ERR_IN_USE EBUSY
#   define //ENOMEM? EAGAIN? <-no mem or our of stuff
#   define
#endif

#endif

