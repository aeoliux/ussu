/*
MIT License

Copyright (c) 2022 Jacek

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "util/authorize.h"
#include "util/cleanup.h"
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <grp.h>
#include <string.h>
#include <pwd.h>

int main(int argc, char **argv) {
	int opt, oldoptind;
	gid_t gid = 0;
	uid_t uid = 0;
	bool shell = false;
	char *name = NULL, *group = NULL;

	if (argv[1] && argv[1][0] == '-') {
		while ((opt = getopt(argc, argv, ":U:G:SCg:u:")) != -1) {
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
				case 'u':
					name = strdup(optarg);
					break;
				case 'g':
					group = strdup(optarg);
					break;
				case 'S':
					shell = true;
					break;
#ifndef PERSIST_DISABLE
				case 'C':
					if (getuid() != 0) {
						fprintf(stderr,
								"Permission denied: persist cleaning is only allowed if UID is equal 0. Now UID is '%u'\n",
								getuid());
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

	if (name) {
		struct passwd *userdata = getpwnam(name);
		free(name);
		if (!userdata) {
			free(group);
			perror("User not found");
			return 1;
		}
		uid = userdata->pw_uid;
		gid = userdata->pw_gid;
	}
	if (group) {
		struct group *groupdata = getgrnam(group);
		free(group);
		if (!groupdata) {
			perror("Group not found");
			return 1;
		}
		gid = groupdata->gr_gid;
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
