#if !defined(UTILS_H_)
#define UTILS_H_
    #include <stdio.h>
    #include <stdlib.h>

    void *mcalloc(size_t dim, size_t type);
    void *mrealloc(void *ptr, size_t size);
#endif