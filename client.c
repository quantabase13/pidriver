#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>

#define PI_DEV "/dev/pi"

int main()
{
    long long sz;
    int32_t s = lround((1<<16) * M_PI);

    char buf[1];
    char write_buf[] = "testing writing";
    int offset = 10000; /* TODO: try test something bigger than the limit */

    int fd = open(PI_DEV, O_RDWR);
    if (fd < 0) {
        perror("Failed to open character device");
        exit(1);
    }

    for (int i = 0; i <= offset; i++) {
        sz = write(fd, write_buf, strlen(write_buf));
        printf("Writing to " PI_DEV ", returned the sequence %lld\n", sz);
    }

    for (int i = 0; i <= offset; i++) {
        lseek(fd, i, SEEK_SET);
        sz = read(fd, buf, 1);
        printf("Reading from " PI_DEV
               " at offset %d, returned the sequence "
               "%lld %d.\n",
               i, sz, s);
    }

    for (int i = offset; i >= 0; i--) {
        lseek(fd, i, SEEK_SET);
        sz = read(fd, buf, 1);
        printf("Reading from " PI_DEV
               " at offset %d, returned the sequence "
               "%lld %d.\n",
               i, sz, s);
    }

    close(fd);
    return 0;
}