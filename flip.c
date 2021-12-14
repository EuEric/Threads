/* 
 * Operating Systems  (2INCO)  Practical Assignment
 * Threaded Application
 *
 * Eric Abraham (1408828)
 * Matthew Biskas (1430033)
 *
 * Grading:
 * Students who hand in clean code that fully satisfies the minimum requirements will get an 8. 
 * Extra steps can lead to higher marks because we want students to take the initiative. 
 * 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h> // for perror()
#include <pthread.h>
#include <string.h>
#include <semaphore.h>

#include "uint128.h"
#include "flip.h"

// create a bitmask where bit at position n is set
#define BITMASK(n) (((uint128_t)1) << (n))

// check if bit n in v is set
#define BIT_IS_SET(v, n) (((v)&BITMASK(n)) == BITMASK(n))

// set bit n in v
#define BIT_SET(v, n) ((v) = (v) | BITMASK(n))

// clear bit n in v
#define BIT_CLEAR(v, n) ((v) = (v) & ~BITMASK(n))

// declare a mutex, and it is initialized as well
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t s_mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t semaphore;

static void bit_flip(int index, int bit);
static void set_all();
static void print_array();
int sem_value;
//FILE *file;
void *routine(void *arg)
{
    //fflush(stdout);
    //fprintf(stderr,"Test from threads\n");
    int i = *(int *)arg;
    //determines how many multiples there are
    //fprintf(file,"%d\n",i);
    int j = 1;
    while (i * j <= NROF_PIECES)
    {
        pthread_mutex_lock(&mutex);
        bit_flip((i * j) / 128, (i * j) % 128); //sets bit (i*j mod 128) of index i*j div 128
        j++;
        pthread_mutex_unlock(&mutex);
    }

    free(arg);
    sem_post(&semaphore);
    //printf("Released semaphore\n");
    pthread_exit(0);
}

int main(void)
{
    // TODO: start threads to flip the pieces and output the results
    // (see thread_test() and thread_mutex_test() how to use threads and mutexes,
    //  see bit_flip() how to manipulate bits in a large integer)

    pthread_t threads[NROF_THREADS];
    sem_init(&semaphore, 0, NROF_THREADS);
    //file = fopen("text.txt","w");

    // create a detached thread that at the end of its life will call V(s) to maintain the inv
    static pthread_attr_t detachedThread;
    pthread_attr_init(&detachedThread);
    pthread_attr_setdetachstate(&detachedThread, PTHREAD_CREATE_DETACHED);

    //sets all bits to 1
    set_all();

    //P(s)

    for (int i = 2; i <= NROF_PIECES; i++)
    {
        int *a = malloc(sizeof(int));
        *a = i;

        //put some thread in there
        sem_wait(&semaphore);
        pthread_mutex_lock(&s_mutex);
        sem_getvalue(&semaphore, &sem_value);
        //printf("Semaphore value %d index should be %d \n", sem_value, (NROF_THREADS - sem_value - 1));
        pthread_mutex_unlock(&s_mutex);
        if (pthread_create(threads + (NROF_THREADS - sem_value - 1), &detachedThread, routine, a) != 0)
        {

            fprintf(stderr, "Oh dear, something went wrong with create! %s\n", strerror(errno));
            return 1;
        }
    }

    // printf("v (all 1's) : %lx%016lx\n", HI(buffer[1]), LO(buffer[1]));
    // print_array();

    // //no longer need to join in case of detached threads

    // for (int i = 0; i < NROF_THREADS; i++)
    // {
    //     if (pthread_join(threads[i], NULL) != 0)
    //     {
    //         fprintf(stderr, "Oh dear, something went wrong with join! %s\n", strerror(errno));
    //         return 1;
    //     }
    // }

    // sem_wait(&semaphore);
    // pthread_mutex_lock(&s_mutex);
    // sem_getvalue(&semaphore, &sem_value);
    // //printf("Nr is %d\n", sem_value);
    // if (sem_value == NROF_THREADS)
    // {
    // }

    //pthread_attr_destroy(&detachedThread);
    sem_destroy(&semaphore);
    print_array();
    //close(file);
    return (0);
}

static void set_all()
{
    for (int i = 0; i < sizeof(buffer) / sizeof(uint128_t); i++)
    {
        buffer[i] = ~0;
        //   printf ("Piece at position %d %lx%016lx\n", i,HI(buffer[i]), LO(buffer[i]));
    }
}

static void print_array()
{
    fflush(stdout);
    for (int i = 1; i <= NROF_PIECES; i++)
    {
        if (BIT_IS_SET(buffer[i / 128], i % 128))
        {
            //used to print to a file
            //fprintf(ptr, "%d ", i);
            //print to stout
            printf("%d\n", i);
        }
    }
}

static void
bit_flip(int index, int bit)
{
    BIT_IS_SET(buffer[index], bit) ? BIT_CLEAR(buffer[index], bit) : BIT_SET(buffer[index], bit);
}
