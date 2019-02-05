/******************************************
 * write_msg.c
 * Kay Zemoudeh
 *
 * Fig 3.17 in Silberschatz OS book 9th Ed.
 * on using shared memory.
 *
 * added mman.h
 *
 * compile: cc -o write_msg write_msg.c -lrt
 *
 * -lrt RunTime library for shm_open()
 * ****************************************/

#include <stdio.h>
//#include <stdlib.h>
#include <string.h>
#include <fcntl.h>		// for O_CREAT O_RDWR
//#include <sys/shm.h>
//#include <sys/stat.h>
#include <sys/mman.h>	// for PROT_WRITE MAP_SHARED
#include <unistd.h>	    // for ftruncate()

int main()
{
	const int SIZE = 4096;
	const char *name = "OS"; // name of shared memory object

	const char *message_0 = "Hello";
	const char *message_1 = "World!";

	int shm_fd;
	void *ptr;

	// create shm obj: shm_fd = shm_open(name, oflags, mode)
	// oflags take precedence over mode
	// mode as always is for user, group, everybody else
	shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);

	ftruncate(shm_fd, SIZE); // resize

	// memory map: ptr = mmap(addr, len, prot, flags, filedes, off)
	// prot (protection) could be PROT_WRITE or PROT_READ ...
	// flags could be MAP_SHARED or MAP_PRIVATE ...
	// off is offset within filedes
	ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

	// write to the obj
	sprintf(ptr, "%s", message_0);
	ptr += strlen(message_0);
	sprintf(ptr, "%s", message_1);
	ptr += strlen(message_1); // not necessary

	return 0;
}
	

