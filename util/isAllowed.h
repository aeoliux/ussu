#ifndef AUTH_ISALLOWED_H
#define AUTH_ISALLOWED_H

#include <stdbool.h>
#include <sys/types.h>

struct permission {
	bool nopasswd;
#ifdef ANTI_BRUTE_FORCE
	unsigned int sec;
#endif
#ifndef PERSIST_DISABLE
	bool persist;
#endif
	bool userpasswd;
};

struct permission *isAllowed(const char *username);

#endif //AUTH_ISALLOWED_H
