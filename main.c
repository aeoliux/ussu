#include "util/authorize.h"
#include "util/cleanup.h"
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv) {
	int opt, oldoptind;
	gid_t gid = 0;
	uid_t uid = 0;
	bool shell = false;

	if (argv[1] && argv[1][0] == '-') {
		while ((opt = getopt(argc, argv, ":U:G:SC")) != -1) {
			bool brk = false;

			if ((optind - oldoptind) == 3) {
				optind -= 3;
				break;
			}

			oldoptind = optind;

			switch (opt) {
				case 'U':
					sscanf(optarg, "%u", &uid);
					break;
				case 'G':
					sscanf(optarg, "%u", &gid);
					break;
				case 'S':
					shell = true;
					break;
#ifndef PERSIST_DISABLE
				case 'C':
					if (getuid() != 0) {
						fprintf(stderr, "Permission denied: persist cleaning is only allowed if UID is equal 0. Now UID is '%u'\n", getuid());
						return 1;
					}
					fullclean();
					return 0;
#endif

				case '?':
					brk = true;
					break;
			}
			if (brk) {
				optind -= 2;
				break;
			}
		}
	} else
		optind = 1;

#ifndef PERSIST_DISABLE
	cleanup();
#endif

	char **args = &argv[optind];
	int args_c = argc - optind;
	if (!args_c && !shell) {
		fprintf(stderr, "Not enough arguments\n");
		return 1;
	}

	if (authorize(uid, gid) == -1) {
		if (show_errno) {
			perror(message);
		} else {
			fputs(message, stderr);
			fputc('\n', stderr);
		}

		return 1;
	}

	if (shell) {
		char *shellcmd[] = {getenv("SHELL"), NULL};
		args = shellcmd;
	}

	execvp(args[0], args);
	perror("failed to execute");
	return 1;
}
