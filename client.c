#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#define PI_DEV "/dev/pi"
#define CLOCK_ID CLOCK_MONOTONIC_RAW
#define ONE_SEC 1000000000.0

int main(int argc, char *argv[])
{
    long long sz;
    char *p;
    int i;
    long conv = strtol(argv[1], &p, 10);
    i = conv;
    char *q;
    int j;
    long conv2 = strtol(argv[2], &q, 10);
    j = conv2;
    int32_t s = lround((1<<16) * M_PI);

    char buf[1];
    char write_buf[] = "testing writing";
    int offset = 100000; /* TODO: try test something bigger than the limit */

    int fd = open(PI_DEV, O_RDWR);
    if (fd < 0) {
        perror("Failed to open character device");
        exit(1);
    }
    
    lseek(fd, i, SEEK_SET);
    sz = write(fd, write_buf, j);
    printf("%lld", sz);
    // double pi_fix = 0;
    // for (int i = 0; i < 50; i++){
    //     double tmp = (double) (sz &1) / ((long long)1 << (50 - i));
    //     sz >>= 1;
    //     pi_fix += tmp; 
    // }
    // pi_fix += sz;
    // printf("%.15f %.15f\n", pi_fix, M_PI);
    
    // for (int i = 0; i <= offset; i++) {
    //     for (int j = 0; j < 100; j++){
    //             struct timespec t1, t2;
    //             lseek(fd, i, SEEK_SET);
    //             clock_gettime(CLOCK_ID, &t1);
    //             sz = write(fd, write_buf, atoi(argv[1]));
    //             clock_gettime(CLOCK_ID, &t2);
    //             long user_time;
    //             if ((t2.tv_nsec-t1.tv_nsec)<0)
    //                 user_time = 1000000000 + t2.tv_nsec - t1.tv_nsec;
    //             else
    //                 user_time = t2.tv_nsec - t1.tv_nsec;
    //             printf("%d %lld %ld %lld\n", i, sz, user_time,
    //                    user_time - sz);
    //     }
    //     // sz = write(fd, write_buf, strlen(write_buf));
    //     // printf("Writing to " PI_DEV ", returned the sequence %lld\n", sz);
    // }

    // for (int i = 0; i <= offset; i++) {
    //     lseek(fd, i, SEEK_SET);
    //     sz = read(fd, buf, 1);
    //     printf("Reading from " PI_DEV
    //            " at offset %d, returned the sequence "
    //            "%lld %d.\n",
    //            i, sz, s);
    // }

    // for (int i = offset; i >= 0; i--) {
    //     lseek(fd, i, SEEK_SET);
    //     sz = read(fd, buf, 1);
    //     printf("Reading from " PI_DEV
    //            " at offset %d, returned the sequence "
    //            "%lld %d.\n",
    //            i, sz, s);
    // }

    close(fd);
    return 0;
}