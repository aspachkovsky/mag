#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/param.h>
#include <string.h>

static ssize_t (*libc_read) (int, void*, size_t) = NULL;

#define ENVNAME "TRUSTED_RANDOM"

ssize_t read(int fd, void *buf, size_t count) {
	char *fd_path, *env_random_str; 
	size_t size, fd_path_size;

	if (libc_read == NULL) {
		libc_read = dlsym(RTLD_NEXT, "read");
    	if (libc_read == NULL) {
        	fprintf(stderr, "dlsym for read error: %s\n", dlerror());
        	goto out;
    	}
    }
   
    char *path = malloc(PATH_MAX * sizeof(char));
    memset(path, (int) '\0', PATH_MAX * sizeof(char));
    
	pid_t pid = getpid();
    sprintf(path, "/proc/%d/fd/%d", pid, fd);

    fd_path = malloc(PATH_MAX * sizeof(char));
    if (fd_path == NULL) {
    	fprintf(stderr, "Failed to malloc");
    	goto out;
    }

    fd_path_size = readlink(path, fd_path, PATH_MAX * sizeof(char));
    if (fd_path_size == -1) {
    	fprintf(stderr, "Failed to readlink");
    	goto out;
    }

    if (strcmp(fd_path, "/dev/random") == 0 || strcmp(fd_path, "/dev/urandom") == 0) {
    	env_random_str = getenv(ENVNAME);
    	if (env_random_str == NULL) {
    		goto out;
    	}

    	size = MIN(count, strlen(env_random_str));
    	strncpy(buf, env_random_str, size);
    	return size;
    }

	out:
		return libc_read(fd, buf, count);
}	

	