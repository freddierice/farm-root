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
#include <linux/fs.h>

#include "shared.h"

int main(int argc, const char *argv[]) {
	int fd; 

#ifdef FARM_PULL
	if( rsf_set(RSF_CACHE_TO_DATA_WORKING) ) {
		LOGV("[*] bridge started");
		
		// spin while the recovery copy has not finished
		while(!rsf_check(RSF_DISK_TO_CACHE_DONE)) sleep(1);

		LOGV("[*] bridge copying image");
		if( copy(DATA_PULL_RECOVERY_IMG, CACHE_PULL_RECOVERY_IMG) ) {
			LOGV("ERROR: could not copy %s to %s", CACHE_PULL_RECOVERY_IMG, DATA_PULL_RECOVERY_IMG);
			return 0;
		}
		
		LOGV("[*] bridge done");
		rsf_set(RSF_CACHE_TO_DATA_DONE);
		rsf_set(RSF_ALL_DONE);
	}
#else
	if( rsf_set(RSF_DATA_TO_CACHE_WORKING) ) {
		LOGV("[*] bridge started");
		LOGV("[*] bridge copying image");
		if( copy(CACHE_PUSH_RECOVERY_IMG, DATA_PUSH_RECOVERY_IMG) ) {
			LOGV("ERROR: could not copy %s to %s", DATA_PUSH_RECOVERY_IMG, CACHE_PUSH_RECOVERY_IMG);
			return 0;
		}

		LOGV("[*] bridge done");
		rsf_set(RSF_DATA_TO_CACHE_DONE);
		
		// wait to let main thread know that the recovery push is complete
		while(!rsf_check(RSF_CACHE_TO_DISK_DONE)) sleep(1);

		rsf_set(RSF_ALL_DONE);
	}
#endif

	return 0;
}
