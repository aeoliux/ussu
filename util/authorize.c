#include "authorize.h"
#include "isAllowed.h"
#include "util.h"
#include <shadow.h>
#include <pwd.h>
#include <crypt.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>

char message[4096];
bool show_errno = false;

int authorize(uid_t uid, gid_t gid) {
	bool pass = false;
	char *passwd = NULL, *username = NULL;

	// Get information about current user
	struct passwd *current_user = getpwuid(getuid());
	if (!current_user) {
		sprintf(message, "failed to fetch information about current user");
		show_errno = true;
		return -1;
	}

	// Check for permission to use runas
	struct permission *info = isAllowed(current_user->pw_name);
	if (!info) {
		show_errno = true;
		sprintf(message, "permission check failed");
		return -1;
	}

	// Set specific for runas env variables
	char env[1024];
	sprintf(env, "USSU_USER=%s", current_user->pw_name);
	putenv(strdup(env));
	sprintf(env, "USSU_UID=%u", current_user->pw_uid);
	putenv(strdup(env));

	// Check if user is allowed to use their password
	if (info->userpasswd) {
		passwd = strdup(current_user->pw_passwd);
		username = strdup(current_user->pw_name);
	}

	// Get information about the user to authorize
	struct passwd *to_authorize = getpwuid(uid);
	if (!to_authorize) {
		show_errno = true;
		sprintf(message, "failed to get information about user with ID '%u'", uid);
		free(info);
		free(passwd);
		free(username);
		return -1;
	}
	// if user isn't allowed to use their password, use user's to authorize password
	if (!info->userpasswd) {
		passwd = strdup(to_authorize->pw_passwd);
		username = strdup(to_authorize->pw_name);
	}

#ifndef PERSIST_DISABLE
	char persist_base[PATH_MAX];
	char path[PATH_MAX];
	sprintf(persist_base, "/run/ussu");
	sprintf(path, "%s/%u", persist_base, getppid());

	if (info->persist && !info->nopasswd) {
		setegid(to_authorize->pw_gid);
		if (is_file(path)) {
			pass = true;
		}
	}
#endif

	if (!info->nopasswd) {
		if (!pass) {
			// If password is equal to 'x', check shadow password
			if (!strcmp(passwd, "x")) {
				free(passwd);

				struct spwd *shadow = getspnam(username);
				if (!shadow) {
					show_errno = true;
					sprintf(message, "failed to get password from shadow database");

					free(info);
					free(username);

					return -1;
				}
				passwd = strdup(shadow->sp_pwdp);
			}

			// Split password
			char **splitted = split(passwd, "$");
			if (!splitted ||
				(splitted[0] == NULL || splitted[1] == NULL || splitted[2] == NULL || splitted[3] == NULL)) {
				free(info);
				free(passwd);
				free(username);
				if (splitted) split_free(splitted);

				sprintf(message, "incorrect password format");
				show_errno = true;

				return -1;
			}

			// Construct salt
			char salt[4096];
			sprintf(salt, "$%s$%s$%s", splitted[0], splitted[1], splitted[2]);

			// Read password
			char *input = getpass("Password: ");
			if (!input) {
				sprintf(message, "couldn't have read password");
				show_errno = true;

				free(info);
				free(passwd);
				free(username);
				split_free(splitted);
				free(input);

				return -1;
			}

			// Crypt typed password
			char *crypted = crypt(input, salt);
			split_free(splitted);

			// Compare crypted user's password with crypted typed password
			if (strcmp(crypted, passwd)) {
				show_errno = false;
				sprintf(message, "invalid password");

				free(info);
				free(passwd);
				free(username);
				return -1;
			}
#ifndef PERSIST_DISABLE
			if (info->persist) {
				bool ok = true;
				if (access(persist_base, F_OK)) {
					if (mkdir(persist_base, 0710)) {
						ok = false;
					}
				}
				if (ok) {
					touch(path, 0640);
				}
			}
#endif
		}
	}

	free(passwd);
	free(info);
	free(username);

	// Set user ID
	if (setuid(uid) == -1) {
		show_errno = true;
		sprintf(message, "failed to set user ID");
		return -1;
	}
	// Set group ID
	if (setgid(gid) == -1) {
		show_errno = true;
		sprintf(message, "failed to set group ID");
		return -1;
	}

	// Set essential env variables
	struct passwd *user = getpwuid(uid);
	if (user) {
		sprintf(env, "USER=%s", user->pw_name);
		putenv(strdup(env));

		sprintf(env, "HOME=%s", user->pw_dir);
		putenv(strdup(env));

		sprintf(env, "SHELL=%s", user->pw_shell);
		putenv(strdup(env));
	}

	return 0;
}