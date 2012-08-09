[OI](http://github.com/geky/oi) Documentation
=============================================

[oi_os](oi/blob/master/oi_os.h)
---------------------------------------------

oi_os is the starting point for OI. It determines your compiling enviroment and sets up internal macros to be used in OI. It may be necessary for some OI file to get included before other includes so oi_os can set up correctly.

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

As well as which language, a trivial check.
* OI_CPP
* OI_C

OI will probably work on other configurations, and more will be added as they are confirmed to work.

[oi_err](oi/blob/master/oi_err.h)
---------------------------------------------

[oi_types](oi/blob/master/oi_types.h)
---------------------------------------------

[oi_time](oi/blob/master/oi_time.h)
---------------------------------------------

- - - - - - - - - - - - - - - - - - - - - - -

[oi_read.h](oi/blob/master/oi_thread.h)
---------------------------------------------

[oi_local](oi/blob/master/oi_local.h)
---------------------------------------------

[oi_mutex](oi/blob/master/oi_mutex.h)
---------------------------------------------

[oi_rwlock](oi/blob/master/oi_rwlock.h)
---------------------------------------------

[oi_cond](oi/blob/master/oi_cond.h)
---------------------------------------------

- - - - - - - - - - - - - - - - - - - - - - -

[oi_pack](oi/blob/master/oi_pack.h)
---------------------------------------------

[oi_net](oi/blob/master/oi_net.h)
---------------------------------------------

[oi_address](oi/blob/master/oi_address.h)
---------------------------------------------

[oi_socket](oi/blob/master/oi_socket.h)
---------------------------------------------

[oi_udp](oi/blob/master/oi_udp.h)
---------------------------------------------

[oi_tcp](oi/blob/master/oi_tcp.h)
---------------------------------------------

