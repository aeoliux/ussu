#ifndef AUTH_AUTHORIZE_H
#define AUTH_AUTHORIZE_H

#include <unistd.h>
#include <stdbool.h>

int authorize(uid_t uid, gid_t gid);

extern char message[4096];
extern bool show_errno;

#endif //AUTH_AUTHORIZE_H
