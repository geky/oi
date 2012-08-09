[OI](http://github.com/geky/oi) - OS Interface
----------------------------------------------

### Overveiw ###

OI is a very tiny and simple library written in C to open up OS specific functions in a light-weight and cross-platform manner. 

Since most of the code simply redirects to OS calls, the files are entirely self contained in their headers. This makes it very easy to add OI to a project as you only have to include the file you need without worrying about the language or additional compiling overhead.

It is encouraged that modify the files so they match your specific needs. If you think your additions are useful, don't hesitate to submit a pull request as any help is greatly appreciated.

### Organization ###

Each file is designed to be as independent as possible so you can only use the files you need. The beginning of every file has a list of include statements for every file it relies on so you can easily tell which ones you need. It will also have a comment if there is any external linkages that are needed. 

OI starts with oi_os.h, which determines the OS information and is required in every other file. Also oi_types.h defines useful ints and floats of set sizes for use throughout OI.

The actual code is organized into many `oi_func` and `oi_call` functions. The oi_func functions can never fail in theory and returns data associated with the function. The oi_call functions return an int indicating any error that occurs, or zero if it is successful. You can get a string explaining the error through the get_error function in oi_err.h. Most errors are very much OS dependent and usually only result from bugs; however, reliable error codes are defined in oi_err.h and indicated in comments before the function.

Each file usually revolves around a specific datatype which uses the `_t` syntax such as mutex_t. The actual implementation changes on each system, making it difficult to use consistent datatypes. You should not rely on the type at all, but rather depend on the functions that use it.

Anything in the files that begins with `_oi` is intended only for internal use, and should not be used. That being said if you need something prefixed with `_oi`, feel free to change its name and submit a pull request to indicate its nescessity.

### Documentation  ###

A full documentation of each file can be found in [docs.md](oi/blob/master/docs.md).

### Testing ###

OI is trying to implement a standard interface for utilities that are common for applications on modern computers. This isn't C's main focus, so most of the implementations of these utilities vary widely from computer to computer. A simple program [test.c](oi/blob/master/test.c) is provided for a quick way to confirm OI works on a target platform. However, it is impossible to cover every OS, so if you find an OS where OI breaks, please let us know, or even implement a fix yourself and submit a pull request.

### Example ###

Here is a simple example of using OI:

    #include "oi_thread.h"
    #include <stdio.h>
    
    void prthread(void * s) {
        printf("%s!",(char*)s);
    }
    
    int main() {
        char data[] = "Hello World";
        thread_t thr;
        int err;
        
        err = thread_create(&thr,&prthread,data);
        thread_join(&thr);
    
        return err;
    }
