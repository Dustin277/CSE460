/**
 * Kay Zemoudeh
 * 4/21/13
 * boundBuf.c
 *
 * Producer-consumer problem using shared memory.
 * Shared memory consists of buffer, in, and out.
 * Producer continously produces an item (integer) and 
 * places it in buffer. Consumer gets items from the buffer.
 * Item is initially 123 and is incremented each time
 * the producer generates a new item.
 * Both producer and consumer sleep between succesive calls 
 * for a random number of seconds to simulate speed 
 * difference between the two processes.
 * In this program each process loops 10 times.
 *
 * Compile: gcc boundBuf.c -lrt
 */

#include <stdio.h>
#include <sys/mman.h>
#include <sys/time.h> // for struct timeval
#include <fcntl.h>
#include <unistd.h>   // for ftruncate()
#include <stdlib.h>   // for srand() rand()
#include <sys/wait.h> // for wait()

#define BUFSIZE 5

int main()
{
    typedef struct {
        int in;
        int out;
        int buffer[BUFSIZE];
    } Seg;
    Seg * shared_memory;

    int segment_fd;

    int pid;
    int i, item = 123;
    int duration;
    struct timeval tv;
    
    segment_fd = shm_open("BoundedBuf", O_CREAT | O_RDWR, 0666);
    ftruncate(segment_fd, (BUFSIZE+2)*4);
    shared_memory = (Seg *) mmap(0, (BUFSIZE+2)*4, PROT_WRITE | PROT_READ, MAP_SHARED, segment_fd, 0);

    shared_memory->in = 0;
    shared_memory->out = 0;

    /* fork() a child process to act as Consumer, the parent acts as Producer.
     * The parent process will wait() on child and then removes shared memory.
     */
    if ( (pid = fork()) == -1) {
        fprintf(stderr,"Couldn't fork off the child, pid =  %d\n", pid);
        return 3;
    }

    if (pid == 0) {
        printf("Child: Consumer\n");

        gettimeofday(&tv, NULL); /* couldn't use time() which returns seconds */
        srand(tv.tv_usec);       /* usecs (micro-secs) will have different seeds for parent and child */ 

        for (i = 0; i < 10; i++) {
            duration = (rand())%7; /* generate a random number in [0,10) */
            sleep(duration);
            printf("\nConsumer slept for %02d secs\n", duration);

            if (shared_memory->in == shared_memory->out) printf("    CONSUMER WAIT LOOP!\n");
            while (shared_memory->in == shared_memory->out) /* do nothing! */ ;

            item = shared_memory->buffer[shared_memory->out];
            printf("Consumer consumed %d from buf[%d]\n", item, shared_memory->out);
            shared_memory->out = (shared_memory->out+1)%BUFSIZE;
        }

        printf("Child: exiting\n");
    }
    else {
        printf("Parent: Producer\n");

        gettimeofday(&tv, NULL);
        srand(tv.tv_usec);

        for (i = 0; i < 10; i++) {
            duration = rand()%5;
            sleep(duration);
            printf("\nProducer slept for %02d secs\n", duration);

            if (((shared_memory->in+1)%BUFSIZE) == shared_memory->out) printf("    PRODUCER WAIT LOOP!\n");
            while (((shared_memory->in+1)%BUFSIZE) == shared_memory->out) /* do nothing! */ ;

            shared_memory->buffer[shared_memory->in] = item++;
            printf("Producer produced %d in buf[%d]\n", shared_memory->buffer[shared_memory->in], shared_memory->in);
            shared_memory->in = (shared_memory->in+1)%BUFSIZE;
        }

        wait(0);
        printf("Parent: exiting\n");
        shm_unlink("BoundedBuf");
    }
    return 0;
}
