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

#include "isAllowed.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <grp.h>
#include <limits.h>

#ifndef PREFIX
#define PREFIX "/"
#endif

bool checkGroup(const char *group, const char *username);

struct permission *isAllowed(const char *username) {
	char *buf = NULL;

	// Get configuration file path
	char confpath[PATH_MAX] = PREFIX "/etc/ussu.conf";

	// Open ussu configuration
	if (!access(confpath, R_OK)) {
		buf = readFile(confpath);
	} else if (!access("/etc/ussu.conf", R_OK)) {
		buf = readFile("/etc/ussu.conf");
	} else {
		// unless it exists, set errno to 'No such file or directory'
		errno = ENOENT;
	}
	if (!buf) return NULL;

	// Split lines
	char **lines = split(buf, "\n");
	free(buf);
	if (!lines) {
		return NULL;
	}

#ifdef ANTI_BRUTE_FORCE
	// Buffer for anti-bruteforce
	unsigned int sec = 0;
#endif

	// Iterate over lines
	for (size_t i = 0; lines[i] != NULL; i++) {
		// Split words
		char **words = split(lines[i], " \t");
		if (!words) {
			split_free(lines);
			return NULL;
		}

		// If first char is #, skip
		if (words[0][0] == '#')
			continue;

		// If user is in specified group, allow them
		bool pass = false;
		if (words[0][0] == ':') {
			if (checkGroup(&words[0][1], username)) {
				pass = true;
			}
		}

		// Tweak class
		if (words[0][0] == '.') {
#ifdef ANTI_BRUTE_FORCE
			// Get seconds for anti-bruteforce
			if (!strcmp(&words[0][1], "anti-bruteforce")) {
				sscanf(words[1], "%u", &sec);
			}
#endif
			continue;
		}

		// Compare first word with the username
		if (!strcmp(words[0], username) || pass) {
			// Allocate permission struct
			struct permission *perm = malloc(sizeof(struct permission) * 1);
			if (!perm) {
				split_free(lines);
				split_free(words);
				return NULL;
			}

			// Set all values to false
#ifndef PERSIST_DISABLE
			perm->persist = false;
#endif
			perm->nopasswd = false;
			perm->userpasswd = false;
#ifdef ANTI_BRUTE_FORCE
			perm->sec = sec;
#endif

			// Iterate over words and set settings
			for (size_t i2 = 1; words[i2] != NULL; i2++) {
#ifndef PERSIST_DISABLE
				if (!strcmp(words[i2], "persist")) {
					perm->persist = true;
				} else
#endif
				if (!strcmp(words[i2], "nopasswd")) {
					perm->nopasswd = true;
				} else if (!strcmp(words[i2], "userpasswd")) {
					perm->userpasswd = true;
				}
			}

			// Return pointer to the struct
			split_free(words);
			split_free(lines);
			return perm;
		}

		split_free(words);
	}

	// If permission isn't granted, set errno to 'Permission denied' and return NULL
	split_free(lines);
	errno = EACCES;
	return NULL;
}

// Function for checking if user is in specified group
bool checkGroup(const char *group, const char *username) {
	struct group *grp = getgrnam(group);
	if (grp) {
		for (unsigned long i = 0; grp->gr_mem[i] != NULL; i++) {
			if (!strcmp(grp->gr_mem[i], username)) {
				return true;
			}
		}
	}

	return false;
}