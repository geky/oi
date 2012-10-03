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

#### Requirements ####
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

`const char * get_error(int err)`  
Returns a null-termintated, system provided error message for an error code returned by an oi_call.

[oi_types](oi_types.h)
---------------------------------------------

oi_types contains the definitions of standard sized numeric types. It is identical to stdint.h but is available outside of C99's standard library.


#### Requirements ####
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

#### Requirements ####
* oi_os
* oi_types

#### Functions ####

`uint64 millis()`  
Returns the current time in milliseconds.


- - - - - - - - - - - - - - - - - - - - - - -


[oi_thread.h](oi_thread.h)
---------------------------------------------

oi_thread contains code for creating and handling threads independantly of the underlying OS. Threads are complex entities not fully described here. If you are new to threads you should research other implementations to understand the bugs that multithreaded code can induce.

#### Requirements ####
* oi_os
* `-pthread` compiler option on posix machines

#### Definitions ####

`thread_t`

#### Functions ####

`thread_create(thread_t * t, void (*routine)(void *), void * arg)`  
Creates a new thread in t, and runs the routine with the argument that is given.

`thread_sleep(unsigned int ms)`  
Makes the current thread sleep the amount given in milliseconds.

`thread_yield()`  
Makes the current thread yield to allow other threads a chance to run.

`thread_join(thread_t * t)`  
Waits for the given thread to complete and then cleans up the thread's resources.

`thread_terminate(thread_t * t)`  
Forcefully destroys the given thread. This can cause a leak in memory if the thread doesn't get a chance to clean up its resources, and it can happen anywhere and lead to surprising bugs. So it should only be used as a last resort, such as closing an application suddenly.

[oi_local](oi_local.h)
---------------------------------------------

oi_local contains code for storing a pointer in a thread unique manner. Each thread veiws a seperate unique value in the local storage.

#### Requirements ####
* oi_os
* `-pthread` compiler option on posix machines

#### Definitions ####

`local_t`

#### Functions ####

`local_create(local_t * l)`  
Initializes the local storage in l. The value will be initialized to a null-pointer for all threads.

`local_set(local_t * l, void * value)`  
Sets the value of the local storage to the value given for only the thread that calls the function.

`void * local_get(local_t * l)`  
Returns the value currently associated with the local storage given.

`local_destroy(local_t * l)`  
Cleans up any resources used by the local storage.

[oi_mutex](oi_mutex.h)
---------------------------------------------

oi_mutex contains the code for cross-platform recursive mutual exclusion locks useful for creating data barriers around data shared between threads.

#### Requirements ####
* oi_os
* `-pthread` compiler option on posix machines

#### Definitions ####

 `mutex_t `

#### Functions ####

`mutex_create(mutex_t * m)`  
Initializes a recursive mutex in m.

`mutex_lock(mutex_t * m)`  
Locks the given mutex, or waits until it is unlocked by another thread.

`mutex_try_lock(mutex_t * m)`  
Locks the given mutex, or returns ERR_IN_USE if it is locked in another thread.

`mutex_unlock(mutex_t * m)`  
Unlocks the given mutex, allowing other threads to lock it.

`mutex_destroy(mutex_t * m)`  
Cleans up resources used by the mutex.

[oi_rwlock](oi_rwlock.h)
---------------------------------------------

oi_rwlock contains code for cross-platform read-write locks that allow mutiple readers to access data if there are no writers currently using the lock, and only allows one writer to get a lock. Please note that these locks are not recursive, and the prioritization scheme is dependent on the underlying system.

#### Requirements ####
* oi_os
* `-pthread` compiler option on posix machines

#### Definitions ####

`rwlock_t`

#### Functions ####

`rwlock_create(rwlock_t * rw)`  
Initialized a non-recursive read-write lock in rw.

`rwlock_read_lock(rwlock_t * rw)`  
Adds a reading lock on the given rwlock, or waits until a writer is unlocked in another threads.

`rwlock_try_read_lock(rwlock_t * rw)`  
Adds a reading lock on the given rwlock, or returns ERR_IN_USE if it is locked by a writer in another thread.

`rwlock_read_unlock(rwlock_t * rw)`  
Unlocks a read lock in rw, allowing a writer to lock it if there are no other readers.

`rwlock_write_lock(rwlock_t * rw)`  
Makes a write lock on the given rwlock, or waits until all readers are unlocked in other threads.

`rwlock_try_write_lock(rwlock_t * rw)`  
Makes a write lock on the given rwlock, or returns ERR_IN_USE if it is locked by any readers in other threads.

`rwlock_write_unlock(rwlock_t * rw)`  
Releases the write lock on rw, allowing readers to lock, or another writer to lock depending on the prioritization scheme of the system.

`rwlock_destroy(rwlock_t * rw)`  
Cleans up resources used by the rwlock.

[oi_cond](oi_cond.h)
---------------------------------------------

oi_cond contains code for cross-platform condition variables that allow threads to sleep in a state that can be awoken by other threads with an atomically locked mutex. Please note that in all implementations of cond, it can be susceptible to spurious wakeups when no thread has signalled the cond.

#### Requirements ####
* oi_os
* oi_mutex
* `-pthread` compiler option on posix machines

#### Definitions ####

`cond_t`

#### Functions ####

`cond_create(cond_t * c)`  
Initializes a condition variable in c.

`cond_wait(cond_t * c, mutex_t * m)`  
Makes the currently running thread sleep until the given cond is signalled by another thread. The mutex is unlocked by cond_wait and then atomically locked before waking up to prevent a signal from being lost while the cond is locked.

`cond_timed_wait(cond_t * c, mutex_t * m, unsigned int ms)`  
Makes the currently running thread sleep for the time given in milliseconds, or the given cond is signalled by another thread. If the cond times out, and error of ERR_TIMEOUT is returned. The mutex is unlocked by cond_wait and then atomically locked before waking up to prevent a signal from being lost while the cond is locked.

`cond_signal_one(cond_t * c)`  
Wakes up a single thread currently waiting on the cond given.

`cond_signal_all(cond_t * c)`  
Wakes up all threads currently waiting on the cond given.

`cond_destroy(cond_t * c)`  
Cleans up resources used by the condition variable.


- - - - - - - - - - - - - - - - - - - - - - -


[oi_pack](oi_pack.h)
---------------------------------------------

The underlying representation of numerical types can vary widely between different systems. oi_pack provides a solution in the form of pack and unpack functions for converting to and from the network representation of different types. Ints are converted to Big-Endian and floats are converted to IEEE representations. If the underlying system is already implemented as such, no conversion is performed.

#### Requirements ####
* oi_os
* oi_types

#### Functions ####

`void pack(void * net, void * source, size_t len)`  
Maps the given source array to the net array. Simply a wrapper for memcpy.

`void unpack(void * net, void * dest, size_t len)`  
Maps the net array to the given dest array. Simply a wrapper for memcpy.

`void pack8(void * net, uint8 source)`  
Maps the given source to the net buffer.

`uint8 unpack8(void * net)`  
Maps the net buffer to an uint8.

`void pack16(void * net, uint16 source)`  
Maps the given source to the net buffer.

`uint16 unpack16(void * net)`  
Maps the net buffer to an uint16.

`void pack32(void * net, uint32 source)`  
Maps the given source to the net buffer.

`uint32 unpack32(void * net)`  
Maps the net buffer to an uint32.

`void pack64(void * net, uint64 source)`  
Maps the given source to the net buffer.

`uint64 unpack64(void * net)`  
Maps the net buffer to an uint64.

`void packf32(void * net, float32 source)`  
Maps the given source to an IEEE repressentation on the net buffer.

`float32 unpack32(void * net)`  
Maps the net buffer to a float32.

`void packf64(void * net, float64 source)`  
Maps the given source to an IEEE repressentation on the net buffer.

`float64 unpack64(void * net)`  
Maps the net buffer to a float64.

[oi_net](oi_net.h)
---------------------------------------------

oi_net is for setting up internal macros for oi files that utilize the systems network. For the most part oi tries to hide the IPv4 and IPv6 differences, but if you need a specific one you can define OI_IPV4 or OI_IPV6 before oi_net's inclussion to restrict network usage to a single version.

#### Requirements ####
* oi_os

#### Definitions ####
Most importantly oi_net determines if your machine supports a dual network stack; defining one of the following if OI_IPV4 or OI_IPV6 is not set.
* OI_DUALSTACK
* OI_SINGLESTACK

[oi_address](oi_address.h)
---------------------------------------------

oi_address contains code dealing with internet addresses. Each address contains an IP address as well as a port number and internal data for different versions.

#### Requirements ####
* oi_os
* oi_types
* oi_net
* `-lws2_32` compiler option on windows

#### Definitions ####

`address_t`

#### Functions ####

`address_from_ipv4(address_t * addr, void * ip_address, uint16 port)`  
Creates an address in addr from the raw IPv4 address and port. The address must have a size of 4.

`address_from_ipv6(address_t * addr, void * ip_address, uint16 port)`  
Creates an address in addr from the raw IPv6 address and port. The address must have a size of 16.

`address_from_name(address_t * addr, const char * name, uint16 port, int lookup`  
Creates an address in addr from the null-terminated string representation and port. If the value of lookup is not zero, a DNS lookup is performed for domain names, otherwise only conversion from the standard numeric notation of IP addresses can be performed. It can return either ERR_NOT_FOUND if name is not found, or ERR_NO_DATA if the name is found but has no data associated with it


`address_from_name_all(address_t * adarray, size_t * len, const char * name, uint16 port, int lookup)`  
Multiple IP addresses can exist for a single domain name. address_all_from_name allows you to retrieve all address representations of a name in an array address_t point to by adarray with length given in len. It then sets len to the number of addresses found. It can return either ERR_NOT_FOUND if name is not found, or ERR_NO_DATA if the name is found but has no data associated with it


`address_loopback(address_t * addr, uint16 port)`  
Creates a loopback address in addr with the port given independent of the IP version.

`address_host(address_t * addr, uint16 port)`  
Creates an address in addr containing the address of the host currently stored in the system and with the port provided.

`address_name(address_t * addr, char * name, size_t len, int lookup)`  
Puts a null-terminated string representation of the address into the buffer name limit by the buffer size in len. If lookup is not zero a DNS lookup occurs to dtermine the domain name, otherwise a numeric representation is used.

[oi_socket](oi_socket.h)
---------------------------------------------

oi_socket contains code dealing with the use of internet sockets. Sockets can be created independently of underlying protocol, but to actually send data you will need to use either oi_tcp or oi_udp for their respective protocols. Sockets are complex entities, so it is encouraged to research each protocol yourself to understand hey they work.

#### Requirements ####
* oi_os
* oi_types
* oi_net
* oi_address
* `-lws2_32` compiler option on windows

#### Definitions ####

`socket_t`

#### Functions ####

`socket_create(socket_t * s, int proto, uint16 port)`  
Creates an internet socket in s. The type is provided in proto and can either be SOCKET_TCP or SOCKET_UDP depending on the protocol that is needed. The socket is then bound to the port given or if port is 0, it will be bound to a system determined port when nescessary. socket_create can return ERR_TAKEN if the given port is already being used by another socket.

`socket_create_on(socket_t * s, int proto, address_t * addr)`  
Creates an internet socket in s. The type is provided in proto and can either be SOCKET_TCP or SOCKET_UDP depending on the protocol that is needed. The socket is then bound to the specific address given by addr. socket_create can return ERR_TAKEN if the given port is already being used by another socket.

`socket_destroy(socket_t * s)`  
Closes any connections used by s and cleans up any resources.

`socket_set_send_buffer(socket_t * s, size_t len)`  
Sets the socket's send-buffer size to the length give.

`size_t socket_get_send_buffer(socket_t * s)`  
Gets the socket's send-buffer size.

`socket_set_rec_buffer(socket_t * s, size_t len)`  
Sets the socket's receive-buffer size to the length give.

`size_t socket_get_rec_buffer(socket_t * s)`  
Sets the socket's receive-buffer size.

[oi_udp](oi_udp.h)
---------------------------------------------

oi_udp contains the socket code specifically for UDP, the User Datagram Protocol.

#### Requirements ####
* oi_os
* oi_types
* oi_net
* oi_address
* oi_socket
* `-lws2_32` compiler option on windows

#### Functions ####

`udp_send(socket_t * s, void * buffer, size_t * len, address_t * addr)`  
Sends the buffer with the length in len through the socket given in s to the address specified by addr. It uses the UDP protocol so the packet that is sent has no garuntee for delivery. The amount sent is then stored in len and may be less than the amount given.

`udp_rec(socket_t * s, void * buffer, size_t * len, address_t * addr)`  
Waits for data to be received on the socket given. When the socket receives data it is stored in the buffer limited by the length in len and the amount of data is stored in len. The address that the data is received from is then stored in addr.

`udp_rec_any(socket_t ** select, void * buffer, size_t * len, address_t * addr, unsigned int ms, int number, socket_t * s1,...)`  
Waits for data to be received on any of the sockets given for the milliseconds given. If the socket receives data it is stored in the buffer limited by the length in len and the amount of data is stored in len. The address that the data is received from is then stored in addr and the socket that receives the data is pointed to by select. If the time runs out, len and select are set to 0 and the functions returns ERR_TIMEOUT.

[oi_tcp](oi_tcp.h)
---------------------------------------------

oi_tcp contains the socket code specifically for TCP, the Transmission Control Protocol.

#### Requirements ####
* oi_os
* oi_types
* oi_net
* oi_address
* oi_socket
* `-lws2_32` compiler option on windows

#### Functions ####

`tcp_connect(socket_t * s, address_t * addr)`  
Connects the specified socket to the address given in addr. It can return ERR_REFUSED if the reciever does not accept the connection, ERR_UNREACHABLE_HOST if the end computer is not reachable, ERR_UNREACHABLE_NET if the network containing the end computer is not reachable, and ERR_TIMEOUT if the connection times out. If an error is returned the socket will be cleaned up completely and require reinitialization.

`tcp_connect_any(socket_t ** select_s, address_t ** select_a, unsigned int ms, int number, socket_t * s1, address_t * a1,...)`  
Waits for a connection on any of the specified sockets to the addresses given or times out after the given milliseconds. The socket and address that make a connection will be pointed to by select_s and select_a. It can return ERR_REFUSED if the reciever does not accept the connection, ERR_UNREACHABLE_HOST if the end computer is not reachable, ERR_UNREACHABLE_NET if the network containing the end computer is not reachable, and ERR_TIMEOUT if the connection times out. All sockets that don't make a connection will be cleaned up completely and require reinitialization.

`tcp_accept(socket_t * s, socket_t * connectsock, address_t * addr)`  
Waits for a new connection on the socket s. When a new connection occurs, connectsock is created with the new connection and the address that is being connected from is stored in addr. 

`tcp_accept_any(socket_t ** select, socket_t * connectsock, address_t * addr, unsigned int ms, int number, socket_t * s1,...)`  
Waits for a new connection on any of the specified sockets for the time given in milliseconds. If a new connection occurs, connectsock is created with the new connection ,the address that is being connected from is stored in addr, and select will point to the socket that was connected to. Otherwise it returns ERR_TIMEOUT.

`tcp_send(socket_t * s, void * buffer, size_t * len)`  
Sends the buffer with the length in len through the socket given in s. It can return ERR_DISCONNECTED if the socket is disconnected, or ERR_TIMEOUT if the connection times out. The actual amount sent is stored in len, and should only be different if an error occured.

`tcp_rec(socket_t * s, void * buffer, size_t * len)`  
Waits for data to be received on the socket given. If the socket receives data it is stored in the buffer limited by the length in len and the amount of data is stored in len. It can return ERR_DISCONNECTED if the socket is disconnected, or ERR_TIMEOUT if the connection times out. If an error is returned len will be set to 0.

`tcp_rec_any(socket_t ** select, void * buffer, size_t * len, unsigned int ms, int number, socket_t * s1,...)`  
Waits for data to be received on any of the specified sockets for the milliseconds given. If a socket receives data it is stored in the buffer limited by the length in len with the amount of data stored in len, and the socket that is received from is pointed to by select. It can return ERR_DISCONNECTED if the socket is disconnected, or ERR_TIMEOUT if the connection times out. If an error is returned select and len will be set to 0.

`tcp_set_nodelay(socket_t * s, int val)`  
Enables or disables nodelay on the socket give. Normally a socket will wait for a decently sized buffer to fill up before sending a packet in tcp. setting val non-zero will enable nodelay and send data immediatly.

`int socket_get_nodelay(socket_t * s)`  
Gets wether or not the current socket has the nodelay option enabled.

`socket_set_keepalive(socket_t * s, size_t len)`  
Enables or disables keepalive on the socket give. If these are enabled, the sockt will send out keepalive packets to insure that the connection is connected. If a connection becomes unconnected, ERR_DISCONNECTED is returned on any recieve or send function. A disconnected state can be determined without keepalive enabled as long as there is data transmission. The time interval is around 2 hours, so it isn't very useful unless you will be connected for long periods of time without sending any data. Setting val non-zero will enable keepalive packets.

`int socket_get_keepalive(socket_t * s)`  
Gets wether or not the current socket has the keepalive option enabled.
