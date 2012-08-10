[OI](http://github.com/geky/oi) Documentation
=============================================

[oi_os](oi_os.h)
---------------------------------------------

oi_os is the starting point for OI. It determines your compiling enviroment and sets up internal macros to be used in OI. It may be necessary for some OI file to get included before other includes so oi_os can set up correctly.

#### Definitions ####

oi_os determines your OS and defines one of
* OI_LINUX
* OI_WIN
* OI_MAC
* OI_BSD
* OI_UNKNOWN_OS

oi_os also determines your which compiler you are using
* OI_GCC
* OI_CLANG
* OI_GCC
* OI_UNKNOWN_CC

As well as which language
* OI_CPP
* OI_C

OI will probably work on other configurations, and more definitions will be added as OI is confirmed to work.

[oi_err](oi_err.h)
---------------------------------------------

oi_err contains error related code. It is not required for any file, but is useful for debugging and determining errors.

#### Requires ####
* oi_os

#### Definitions ####

oi_err contains the definitions of errors that can be relied upon during runtime. Other error codes should really only be used to debug a program and vary widely accross configurations.
* ERR_IN_USE
* ERR_TIMEOUT
* ERR_NOT_FOUND
* ERR_NO_DATA
* ERR_TAKEN
* ERR_REFUSED
* ERR_UNREACHABLE_HOST
* ERR_UNREACHABLE_NET
* ERR_DISCONNECTED

#### Functions ####

`oi_func const char * get_error(int err)` Returns a null-termintated, system provided error message for an error code.

[oi_types](oi_types.h)
---------------------------------------------

oi_types contains the definitions of standard sized numeric types. It is identical to stdint.h but is available outside of C99's standard library.


#### Requires ####
* oi_os

#### Definitions ####

oi_types contains the standard numeric types
* int8
* int16
* int32
* int64
* uint8
* uint16
* uint32
* uint64
* float32
* float64

as well as a few floating point definitions
* INFINITY
* NAN

[oi_time](oi_time.h)
---------------------------------------------

oi_time provides a function to get a millisecond timestamp useful for time dependant code.

#### Requires ####
* oi_os
* oi_types

#### Functions ####

`oi_func uint64 millis()` Returns the current time in milliseconds.


- - - - - - - - - - - - - - - - - - - - - - -


[oi_thread.h](oi_thread.h)
---------------------------------------------

oi_thread contains code for creating and handling threads independantly of the underlying OS.

#### Requires ####
* oi_os
* `-pthread` linkage compiler option on posix machines

#### Definitions ####

thread type `thread_t`

#### Functions ####

`oi_call thread_create(thread_t * thread, void (*routine)(void *), void * arg)` Runs the routine in a newly created thread and passes it the arg.

`oi_call thread_create(thread_t * thread, void (*routine)(void *), void * arg)`
  Runs the routine in a newly created thread and passes it the arg.

`oi_call thread_create(thread_t * thread, void (*routine)(void *), void * arg)`  
Runs the routine in a newly created thread and passes it the arg.

    oi_call thread_create(thread_t * thread, void (*routine)(void *), void * arg)` 
        Runs the routine in a newly created thread and passes it the arg.

`oi_call thread_create(thread_t * thread, void (*routine)(void *), void * arg)`
* Runs the routine in a newly created thread and passes it the arg.

`oi_call thread_create(thread_t * thread, void (*routine)(void *), void * arg)` 
    * Runs the routine in a newly created thread and passes it the arg.



[oi_local](oi_local.h)
---------------------------------------------

[oi_mutex](oi_mutex.h)
---------------------------------------------

[oi_rwlock](oi_rwlock.h)
---------------------------------------------

[oi_cond](oi_cond.h)
---------------------------------------------


- - - - - - - - - - - - - - - - - - - - - - -


[oi_pack](oi_pack.h)
---------------------------------------------

[oi_net](oi_net.h)
---------------------------------------------

[oi_address](oi_address.h)
---------------------------------------------

[oi_socket](oi_socket.h)
---------------------------------------------

[oi_udp](oi_udp.h)
---------------------------------------------

[oi_tcp](oi_tcp.h)
---------------------------------------------

