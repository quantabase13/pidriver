
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h> 

/* global variables (shared by all threads */
volatile long long pi = 0; /* the approximation, to 31 sigfigs */
pthread_mutex_t piLock;        /* how we synchronize writes to 'pi' */
long double intervals;         /* how finely we chop the integration */
int numThreads;                /* how many threads we use */

/* the function a thread executes
 * Parameters: arg, a void* storing the address of the thread ID.
 */

struct thread_data{
    long start, end;
};

static void *bbp(long arg)
{
    int k = arg;
    int64_t sum = (4 *(((long long) 1)<<50)/ (8 * k + 1)) - (2 *(((long long) 1)<<50)/ (8 * k + 4)) -
                 (1 *(((long long) 1)<<50)/ (8 * k + 5)) - (1 *(((long long) 1)<<50)/ (8 * k + 6));
    long long *product = malloc(sizeof(long long ));
    if (product)
        *product = 1 / (1 << (4*k)) * sum;
    return (void *) product;
}
void * kthread_compute_pi_bbp(void *data)
{
    int64_t localSum = 0;
    struct thread_data *d = (struct thread_data *)(data);
    long i = d->start;
    long  end = d->end;
    for (; i < end; i++) {
        localSum += *(long long *)bbp(i);
    }
    pthread_mutex_lock(&piLock);
    pi += (localSum);
    pthread_mutex_unlock(&piLock); 
    return NULL;  
}

void * kthread_compute_pi(void *data)
{
    int64_t localSum = 0;
    struct thread_data *d = (struct thread_data *)(data);
    long i = d->start;
    long  end = d->end;
    for (; i < end; i++) {
        int64_t tmp = (i & 1) ? (-1) : 1;
        localSum += tmp * ((((long long) 1)<<50)/(2 * i + 1));
    }
    pthread_mutex_lock(&piLock);
    pi += (localSum << 2);
    pthread_mutex_unlock(&piLock); 
    return NULL;
}



int main(int argc, char **argv)
{
    pthread_t *threads;        /* dynarray of threads */
    void *retval;              /* unused; required for join() */
    struct thread_data *datas;             /* dynarray of thread id #s */
    int i;                     /* loop control variable */
    int terms;

   if (argc == 3) {
      terms= atoi(argv[1]); 
      numThreads = atoi(argv[2]); 
      threads = malloc(numThreads*sizeof(pthread_t));
      datas = malloc(numThreads*sizeof(struct thread_data));
      int *threadID = malloc(numThreads*sizeof(int));
      long long step = terms/numThreads ;
      datas[0].start = 0;
      datas[0].end = step;
      if (numThreads >= 2){
        for (int i = 1; i < numThreads-1; i++){
            datas[i].start = datas[i-1].end;
            datas[i].end = datas[i].start + step;
        }
        datas[numThreads-1].start = datas[numThreads-2].end;
        datas[numThreads-1].end = terms;       
      }
      for (int i = 0; i < numThreads; i++){
        threadID[i] = i;
      }
      pthread_mutex_init(&piLock, NULL);
      struct timespec t1, t2;
      clock_gettime(CLOCK_MONOTONIC, &t1);
      for (i = 0; i < numThreads; i++) {
        pthread_create(&threads[i], NULL, kthread_compute_pi, (void *)(datas+i));
        // pthread_create(&threads[i], NULL, kthread_compute_pi_bbp, (void *)(datas+i));
      }

      for (i = 0; i < numThreads; i++) {
         pthread_join(threads[i], &retval);
      }
      clock_gettime(CLOCK_MONOTONIC, &t2);
        long user_time;
  if ((t2.tv_nsec-t1.tv_nsec)<0)
      user_time = 1000000000 + t2.tv_nsec - t1.tv_nsec;
  else
      user_time = t2.tv_nsec - t1.tv_nsec;
  printf("%ld", user_time);
    //   printf("Estimation of pi is %lld \n", pi);
    // double pi_fix = 0;
    // for (int i = 0; i < 50; i++){
    //     double tmp = (double) (pi &1) / ((long long)1 << (50 - i));
    //     pi >>= 1;
    //     pi_fix += tmp; 
    // }
    // pi_fix += pi;
    // printf("%.15f\n", pi_fix);
      // printf("(actual pi value is 3.141592653589793238462643383279...)\n");
    } else {
      printf("Usage: ./a.out <numIntervals> <numThreads>");    
    }

    return 0;
}
