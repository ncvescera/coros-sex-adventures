#if !defined(RECURSIVEDIR_H_)
#define RECURSIVEDIR_H_
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <errno.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <dirent.h>
    #include <unistd.h>
    #include <time.h>
    
    #define BUFFERSIZE 1024

    struct stat get_file_info(const char *file);
    char *get_cwd();
    void change_dir(const char *path);
    DIR* open_dir(const char *path);
    void close_dir(DIR *dir);
    const char *get_file_ext(const char *filename);
    int isDot(const char dir[]);

    void lsRecursiveDirectory(const char *nomedirectory, const char *extention);

#endif
