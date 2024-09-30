#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    #define OS_WINDOWS 1
    #define WAIT_TIME 2000
#else
    #include <unistd.h>
    #define OS_WINDOWS 0
    #define WAIT_TIME 1
#endif


