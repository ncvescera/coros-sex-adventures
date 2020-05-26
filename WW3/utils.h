#if !defined(UTILS_H_)
#define UTILS_H_
    #include <stdio.h>
    #include <stdlib.h>

    #define MAX_INPUT_SIZE 256
    #define UNIX_PATH_MAX 108

    void *mcalloc(size_t dim, size_t type);
    void *mrealloc(void *ptr, size_t size);
#endif