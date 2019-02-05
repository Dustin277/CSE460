#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    int x;
    int pd[2]; // pipe/file descriptors

    if (pipe(pd) == -1)
    {
        printf("Pipe creation failed.\n");
        exit(1);
    }

    x = fork();

    if (x < 0) 
    {
        printf("Fork failed\n");
        exit(1);
    }

    if (x == 0) 
    { 
        // child
        printf("Child: fork() returned = %d\n", x);

        char get;

        if (read(pd[0], &get, 1) <= 0) {
            printf("read error\n");
            exit(3);
        }

        printf("Child: unblocked, got = %c\n", get);
    } 
    else // x > 0 
    { 
        // parent
        printf("Parent: fork() returned = %d\n", x);

        printf("Parent: input a character: ");
        char y;
        scanf("%c", &y);

        if (write(pd[1], &y, 1) <= 0) {
            printf("write error\n");
            exit(2);
        }

        printf("Parent: terminating\n");
    }
} // main
