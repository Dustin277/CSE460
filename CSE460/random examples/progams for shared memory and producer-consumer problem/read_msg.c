/******************************************
 * read_msg.c
 * Kay Zemoudeh
 *
 * Fig 3.17 in Silberschatz OS book 9th Ed.
 * on using shared memory.
 *
 * added mman.h
 *
 * compile: cc -o read_msg read_msg.c -lrt
 * ****************************************/

#include <stdio.h>
//#include <stdlib.h>
#include <fcntl.h>
//#include <sys/shm.h>
//#include <sys/stat.h>
#include <sys/mman.h>

int main()
{
	const int SIZE = 4096;
	const char *name = "OS"; // name of shared memory object

	int shm_fd;
	void *ptr;

	// open shm obj
	shm_fd = shm_open(name, O_RDONLY, 0222); // it was 0666 in the text

	ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0); // memory map the obj

	// read from the obj
	printf("%s", (char *) ptr);

	shm_unlink(name);

	return 0;
}
	

