#ifndef __SHARED_H__
#define __SHARED_H__

#include <android/log.h>

// recovery status files
extern const char *RSF_DISK_TO_CACHE_WORKING;
extern const char *RSF_DISK_TO_CACHE_DONE;
extern const char *RSF_CACHE_TO_DISK_WORKING;
extern const char *RSF_CACHE_TO_DISK_DONE; 
extern const char *RSF_DATA_TO_CACHE_WORKING;
extern const char *RSF_DATA_TO_CACHE_DONE; 
extern const char *RSF_CACHE_TO_DATA_WORKING;
extern const char *RSF_CACHE_TO_DATA_DONE;
extern const char *RSF_ALL_DONE;

// recovery files
extern const char *DISK_RECOVERY_IMG;
extern const char *CACHE_PULL_RECOVERY_IMG;
extern const char *CACHE_PUSH_RECOVERY_IMG;
extern const char *DATA_PULL_RECOVERY_IMG;
extern const char *DATA_PUSH_RECOVERY_IMG;

// returns 1 if set, or 0 if it exists.
int rsf_set(const char *file);

// returns 1 if exists, or 0 if it does not exist
int rsf_check(const char *file);

// returns 0 on success, other on failure
int rsf_unset(const char *file);

// returns 0 on success, other on failure 
int copy(const char *dst, const char *src);

// log info
#define LOGV(...) { __android_log_print(ANDROID_LOG_INFO, "farm-root", __VA_ARGS__); }

// log notice
#define LOGN(x) { __android_log_print(ANDROID_LOG_INFO, "farm-root", "NOTICE: %s", (x)); }
#endif
