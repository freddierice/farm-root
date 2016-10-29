#include "shared.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sendfile.h>

const char *RSF_DISK_TO_CACHE_WORKING = "/cache/recovery/.disk_to_cache_working";
const char *RSF_DISK_TO_CACHE_DONE    = "/cache/recovery/.disk_to_cache_done";
const char *RSF_CACHE_TO_DISK_WORKING = "/cache/recovery/.cache_to_disk_working";
const char *RSF_CACHE_TO_DISK_DONE    = "/cache/recovery/.cache_to_disk_done";
const char *RSF_DATA_TO_CACHE_WORKING = "/cache/recovery/.data_to_cache_working";
const char *RSF_DATA_TO_CACHE_DONE    = "/cache/recovery/.data_to_cache_done";
const char *RSF_CACHE_TO_DATA_WORKING = "/cache/recovery/.cache_to_data_working";
const char *RSF_CACHE_TO_DATA_DONE    = "/cache/recovery/.cache_to_data_done";
const char *RSF_ALL_DONE              = "/data/local/tmp/.farm_done";

// recovery files
const char *DISK_RECOVERY_IMG       = "/dev/block/bootdevice/by-name/recovery";
const char *CACHE_PULL_RECOVERY_IMG = "/cache/recovery/recovery_pull.img";
const char *CACHE_PUSH_RECOVERY_IMG = "/cache/recovery/recovery_push.img";
const char *DATA_PULL_RECOVERY_IMG  = "/data/local/tmp/recovery_pull.img";
const char *DATA_PUSH_RECOVERY_IMG  = "/data/local/tmp/recovery_push.img";


int rsf_check(const char *file) {
	int fd;
	if( (fd = open(file, O_RDONLY)) == -1 ) {
		return 0;
	}
	close(fd);
	return 1;
}

int rsf_set(const char *file) {
	int fd;

	if(rsf_check(file)) 
		return 0;

	if( (fd = open(file, O_WRONLY | O_TRUNC | O_CREAT, 0777)) == -1 )
		return 0;
	close(fd);

	chmod(file, 0777);
	return 1;
}

int rsf_unset(const char *file) {
	return unlink(file);
}

int copy(const char *dst, const char *src) {
	int dst_fd, src_fd; 
	size_t src_len;
	ssize_t total;

	// open files
	dst_fd = open(dst, O_WRONLY | O_TRUNC | O_CREAT, 0777);
	src_fd = open(src, O_RDONLY);

	// check for open errors
	if( dst_fd == -1 || src_fd == -1 ) {
		if( dst_fd != -1 )
			close(dst_fd);
		if( src_fd != -1 )
			close(src_fd);
		return -1;
	}

	// get src_len
	if( (src_len = (size_t) lseek(src_fd, 0, SEEK_END)) == (size_t)-1 ) {
		close(src_fd); close(dst_fd);
		return -1;
	}
	lseek(src_fd, 0, SEEK_SET);

	// lazy copy file
	total = sendfile(dst_fd, src_fd, NULL, src_len);
	close(dst_fd);
	close(src_fd);

	chmod(dst, 0701);
	chmod(dst, 0702);
	chmod(dst, 0703);
	chmod(dst, 0704);
	chmod(dst, 0705);
	chmod(dst, 0706);
	chmod(dst, 0707);

	// return 0 on success
	return total != (ssize_t)src_len;
}
