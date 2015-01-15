
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void print_numbers(int fd, int count) {
	size_t size = 32 * sizeof(char);
	char *random = malloc(size);
	int i;
	for (i = 0; i < count; i++) {
		read(fd, random, size);	
		printf("%s\n", random);
	}	
	free(random);
}

int main(int argc, char** argv) {

	int fd = open("/dev/random", O_RDWR);
	print_numbers(fd, 5);
	close(fd);

	fd = open("/dev/urandom", O_RDWR);
	print_numbers(fd, 5);
	close(fd);

}