#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>   //mmap, shm_open
#include <fcntl.h>      // O_* constant
#include <sys/types.h>  //ftruncate - setting of length
#include <sys/stat.h>   //fstat - file status, mode const
#include <errno.h>

#define snzprintf(dst, fmt, data...) do { \
        size_t sz = sizeof(dst); \
        memset(dst, 0, sz); \
        snprintf(dst, (sz), fmt, data); \
    } while(0)

int shmem_open(char *filename)
{
    int         fd = -1, err;
    int         ac_mode = O_RDWR;
    bool        new = false;
    char        fname[128];
    struct stat st;

    snzprintf(fname, "/dev/shm/%s", filename);

    if(-1 == stat((const char *)fname, &st)) {
        err = errno;
        if (ENOENT != err) { /* && READ_MODE*/
            printf("Error in creating object "
                "err: %d - (%s)\n",
                errno, strerror(errno));
            goto bailout;
        }
        printf("shmem-filename : %s\n", fname);
        new = true;
        ac_mode |= O_CREAT;
    }

    if(0 > (fd = shm_open(filename, ac_mode, S_IRUSR | S_IWUSR))) {
        printf("Error in creating shm_open "
                "err: %d - (%s)\n",
                errno, strerror(errno));
        goto bailout;
    }

    if(new) {
        printf("new mmap create\n");
    } else {
        printf("mmap opened in read mode\n");
    }

    return 0;
bailout:
    return -1;
}

int main()
{
    shmem_open("hello");
}
