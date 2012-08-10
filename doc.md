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

`oi_func const char * get_error(int err)`  
Returns a null-termintated, system provided error message for an error code returned by an oi_call.

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

`oi_func uint64 millis()`  
Returns the current time in milliseconds.


- - - - - - - - - - - - - - - - - - - - - - -


[oi_thread.h](oi_thread.h)
---------------------------------------------

oi_thread contains code for creating and handling threads independantly of the underlying OS. Threads are complex entities not fully described here. If you are new to threads you should research other implementations to understand the bugs that multithreaded code can induce.

#### Requires ####
* oi_os
* `-pthread` compiler option on posix machines

#### Definitions ####

`thread_t`

#### Functions ####

`oi_call thread_create(thread_t * t, void (*routine)(void *), void * arg)`  
Creates a new thread in t, and runs the routine with the argument that is given.

`oi_call thread_sleep(unsigned int ms)`  
Makes the current thread sleep the amount given in milliseconds.

`oi_call thread_yield()`  
Makes the current thread yield to allow other threads a chance to run.

`oi_call thread_join(thread_t * t)`  
Waits for the given thread to complete and then cleans up the thread's resources.

`oi_call thread_terminate(thread_t * t)`  
Forcefully destroys the given thread. This can cause a leak in memory if the thread doesn't get a chance to clean up its resources, and it can happen anywhere and lead to surprising bugs. So it should only be used as a last resort, such as closing an application suddenly.

[oi_local](oi_local.h)
---------------------------------------------

oi_local contains code for storing a pointer in a thread unique manner. Each thread veiws a seperate unique value in the local storage.

#### Requires ####
* oi_os
* `-pthread` compiler option on posix machines

#### Definitions ####

`local_t`

#### Functions ####

`oi_call local_create(local_t * l)`  
Initializes the local storage in l. The value will be initialized to a null-pointer for all threads.

`oi_call local_set(local_t * l, void * value)`  
Sets the value of the local storage to the value given for only the thread that calls the function.

`oi_func void * local_get(local_t * l)`  
Returns the value currently associated with the local storage given.

`oi_call local_destroy(local_t * l)`  
Cleans up any resources used by the local storage.

[oi_mutex](oi_mutex.h)
---------------------------------------------

oi_mutex contains the code for cross-platform recursive mutual exclusion locks useful for creating data barriers around data shared between threads.

#### Requires ####
* oi_os
* `-pthread` compiler option on posix machines

#### Definitions ####

 `mutex_t `

#### Functions ####

`oi_call mutex_create(mutex_t * m)`  
Initializes a recursive mutex in m.

`oi_call mutex_lock(mutex_t * m)`  
Locks the given mutex, or waits until it is unlocked by another thread.

`oi_call mutex_try_lock(mutex_t * m)`  
Locks the given mutex, or returns ERR_IN_USE if it is locked in another thread.

`oi_call mutex_unlock(mutex_t * m)`  
Unlocks the given mutex, allowing other threads to lock it.

`oi_call mutex_destroy(mutex_t * m)`  
Cleans up resources used by the mutex.

[oi_rwlock](oi_rwlock.h)
---------------------------------------------

oi_rwlock contains code for cross-platform read-write locks that allow mutiple readers to access data if there are no writers currently using the lock, and only allows one writer to get a lock. Please note that these locks are not recursive, and the prioritization scheme is dependent on the underlying system.

#### Requires ####
* oi_os
* `-pthread` compiler option on posix machines

#### Definitions ####

`rwlock_t`

#### Functions ####

`oi_call rwlock_create(rwlock_t * rw)`  
Initialized a non-recursive read-write lock in rw.

`oi_call rwlock_read_lock(rwlock_t * rw)`  
Adds a reading lock on the given rwlock, or waits until a writer is unlocked in another threads.

`oi_call rwlock_try_read_lock(rwlock_t * rw)`  
Adds a reading lock on the given rwlock, or returns ERR_IN_USE if it is locked by a writer in another thread.

`oi_call rwlock_read_unlock(rwlock_t * rw)`  
Unlocks a read lock in rw, allowing a writer to lock it if there are no other readers.

`oi_call rwlock_write_lock(rwlock_t * rw)`  
Makes a write lock on the given rwlock, or waits until all readers are unlocked in other threads.

`oi_call rwlock_try_write_lock(rwlock_t * rw)`  
Makes a write lock on the given rwlock, or returns ERR_IN_USE if it is locked by any readers in other threads.

`oi_call rwlock_write_unlock(rwlock_t * rw)`  
Releases the write lock on rw, allowing readers to lock, or another writer to lock depending on the prioritization scheme of the system.

`oi_call rwlock_destroy(rwlock_t * rw)`  
Cleans up resources used by the rwlock.

[oi_cond](oi_cond.h)
---------------------------------------------

oi_cond contains code for cross-platform condition variables that allow threads to sleep in a state that can be awoken by other threads with an atomically locked mutex. Please note that in all implementations of cond, it can be susceptible to spurious wakeups when no thread has signalled the cond.

#### Requires ####
* oi_os
* oi_mutex
* `-pthread` compiler option on posix machines

#### Definitions ####

`cond_t`

#### Functions ####

`oi_call cond_create(cond_t * c)`  
Initializes a condition variable in c.

`oi_call cond_wait(cond_t * c, mutex_t * m)`  
Makes the currently running thread sleep until the given cond is signalled by another thread. The mutex is unlocked by cond_wait and then atomically locked before waking up to prevent a signal from being lost while the cond is locked.

`oi_call cond_timed_wait(cond_t * c, mutex_t * m, unsigned int ms)`  
Makes the currently running thread sleep for the time given in milliseconds, or the given cond is signalled by another thread. If the cond times out, and error of ERR_TIMEOUT is returned. The mutex is unlocked by cond_wait and then atomically locked before waking up to prevent a signal from being lost while the cond is locked.

`oi_call cond_signal_one(cond_t * c)`  
Wakes up a single thread currently waiting on the cond given.

`oi_call cond_signal_all(cond_t * c)`  
Wakes up all threads currently waiting on the cond given.

`oi_call cond_destroy(cond_t * c)`  
Cleans up resources used by the condition variable.


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

