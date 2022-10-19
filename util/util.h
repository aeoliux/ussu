#ifndef AUTH_UTIL_H
#define AUTH_UTIL_H

#include <sys/types.h>
#include <stdbool.h>

char **split(const char *str, const char *splitter);

void split_free(char **buf);

char *readFile(const char *path);

int touch(const char *path, mode_t perms);

bool is_file(const char *path);

#endif //AUTH_UTIL_H
