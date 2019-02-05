#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    int x, y;

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
        printf("Child: pid = %d\n", getpid());
        scanf("%d", &y);
        printf("Child: entered = %d\n", y);
    } 
    else // x > 0 
    { 
        // parent
        printf("Parent: fork() returned = %d\n", x);
        printf("Parent: pid = %d\n", getpid());
        scanf("%d", &y);
        printf("Parent: entered = %d\n", y);
    }
} // main
