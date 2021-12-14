/* 
 * Operating Systems  (2INCO)  Practical Assignment
 * Threaded Application
 *
 * STUDENT_NAME_1 (STUDENT_NR_1)
 * STUDENT_NAME_2 (STUDENT_NR_2)
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

#include "uint128.h"
#include "flip.h"

static void bit_flip(int index, int bit);
static void set_all(); 
static void print_array();
void *routine(void* arg)
{
    printf("Test from threads\n");
    int i = *(int *)arg;
     //determines how many multiples there are
        int j = 1;
        while (i * j <= NROF_PIECES)
        {
            bit_flip((i * j) / 128, (i * j) % 128); //sets bit (i*j mod 128) of index i*j div 128
            j++;
        }
    
    free(arg);

}

int main(void)
{
    // TODO: start threads to flip the pieces and output the results
    // (see thread_test() and thread_mutex_test() how to use threads and mutexes,
    //  see bit_flip() how to manipulate bits in a large integer)

    pthread_t threads[NROF_THREADS];

    //sets all bits to 1
    set_all();

    for (int i = 0; i < NROF_THREADS; i++)
    {
        int *a = malloc(sizeof(int));
            *a = i;
        if (pthread_create(threads + i, NULL, routine, a))
        {
            
            printf(stderr, "Creation of thread %d failed\n", i);
            return 1;
        }
    }

  
    
    bit_flip(1, 1);
    // printf("%d\n",buffer[1]);
    for (int i = 2; i <= NROF_PIECES; i++)
    {
        //determines how many multiples there are
        int j = 1;
        while (i * j <= NROF_PIECES)
        {
            bit_flip((i * j) / 128, (i * j) % 128); //sets bit (i*j mod 128) of index i*j div 128
            j++;
        }
    }
    printf("v (all 1's) : %lx%016lx\n", HI(buffer[1]), LO(buffer[1]));
    print_array();

      for (int i = 0; i < NROF_THREADS; i++)
    {
        if (pthread_join(threads[i], NULL))
        {
            printf(stderr, "Failed to join with thread %d \n", i);
            return 1;
        }
    }


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

    for (int i = 0; i < sizeof(buffer) / sizeof(uint128_t); i++)
        printf("Piece at position %d %lx%016lx\n", i, HI(buffer[i]), LO(buffer[i]));
}

static void
bit_flip(int index, int bit)
{
    BIT_IS_SET(buffer[index], bit) ? BIT_CLEAR(buffer[index], bit) : BIT_SET(buffer[index], bit);
}
