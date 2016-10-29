#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <android/log.h>
#include <selinux/selinux.h>
#include <cutils/properties.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#include "shared.h"

// #define FARM_PULL 1

int main(int, char **);

static int toolbox_main(int argc, char **argv)
{
    // "toolbox foo ..." is equivalent to "foo ..."
    if (argc > 1) {
        return main(argc - 1, argv + 1);
    } else {
        printf("Toolbox!\n");
        return 0;
    }
}

#define TOOL(name) int name##_main(int, char**);
#include "tools.h"
#undef TOOL

static struct 
{
    const char *name;
    int (*func)(int, char**);
} tools[] = {
    { "toolbox", toolbox_main },
#define TOOL(name) { #name, name##_main },
#include "tools.h"
#undef TOOL
    { 0, 0 },
};

static void SIGPIPE_handler(int signal) {
    // Those desktop Linux tools that catch SIGPIPE seem to agree that it's
    // a successful way to exit, not a failure. (Which makes sense --- we were
    // told to stop by a reader, rather than failing to continue ourselves.)
    _exit(0);
}

int main(int argc, char **argv)
{
    int i;
	char *currcon;
    char *name = argv[0];

	// contexts
	const char *SERVER_CONTEXT = "u:r:system_server:s0";
	const char *INSTALL_CONTEXT = "u:r:install_recovery:s0";

	// only run SERVER_CONTEXT once
	const char *RSF_TOOLBOX = "/cache/recovery/.toolbox";
	
	getcon(&currcon);
	LOGV("[+] toolbox running %s as uid: %d, gid: %d, context %s", argv[0], getuid(), getgid(), currcon);
	
	if( !strcmp(SERVER_CONTEXT, currcon) && rsf_set(RSF_TOOLBOX) ) {
		LOGV("[*] toolbox launching install context");
		property_set("ctl.start", "flash_recovery");
#ifdef FARM_PULL
	}else if( !strcmp(INSTALL_CONTEXT, currcon) && rsf_set(RSF_DISK_TO_CACHE_WORKING) ) {
		LOGV("[*] toolbox pulling recovery");
		if(!fork()) { // lazy, but effective.
			const char *args[] = {"/system/bin/dd", "if=/dev/block/bootdevice/by-name/recovery", "of=/cache/recovery/recovery_pull.img", "bs=10m", 0};
			execv(args[0], (char * const*)args);
		}
		waitpid(-1, NULL, 0);
		LOGV("[*] toolbox done copying");
		rsf_set(RSF_DISK_TO_CACHE_DONE);
	}
#else
	}else if( !strcmp(INSTALL_CONTEXT, currcon) && rsf_set(RSF_CACHE_TO_DISK_WORKING) ) {
		LOGV("[*] toolbox pushing recovery");
		if(!fork()) { // lazy, but effective.
			const char *args[] = {"/system/bin/dd", "if=/cache/recovery/recovery_push.img", "of=/dev/block/bootdevice/by-name/recovery", "bs=10m", 0};
			execv(args[0], (char * const*)args);
		}
		waitpid(-1, NULL, 0);
		LOGV("[*] toolbox done copying");
		rsf_set(RSF_CACHE_TO_DISK_DONE);
	}
#endif


leave_hack:
	free(currcon);

    // Let's assume that none of this code handles broken pipes. At least ls,
    // ps, and top were broken (though I'd previously added this fix locally
    // to top). We exit rather than use SIG_IGN because tools like top will
    // just keep on writing to nowhere forever if we don't stop them.
    signal(SIGPIPE, SIGPIPE_handler);

    if((argc > 1) && (argv[1][0] == '@')) {
        name = argv[1] + 1;
        argc--;
        argv++;
    } else {
        char *cmd = strrchr(argv[0], '/');
        if (cmd)
            name = cmd + 1;
    }

    for(i = 0; tools[i].name; i++){
        if(!strcmp(tools[i].name, name)){
            return tools[i].func(argc, argv);
        }
    }

    printf("%s: no such tool\n", argv[0]);
    return -1;
}
