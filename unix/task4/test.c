#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void usage(void) {
	printf("Usage: test [-c strings_count] [-s string_size] [-i iterations_count] strings... \n");
	printf("Writes strings to /dev/lexsorter device and perform <iterations_count> iterations of\n");
	printf("reading <string_count> strings (a string size if <string_size>) from /dev/lexsorter device.\n");
	printf("-c    strings_count    the number of strings to be read (10 by default), optional.\n");
	printf("-s    string_size      the size of a string in bytes (256 by default), optional.\n");
	printf("-i    iterations_count the number of iterations to be performed (1 by default).\n");
	printf("-h    prints this message.\n");
}

int main(int argc, char** argv) {
	int i, fd, strings_count = 10, string_size = 256, iterations_count = 1, size;

	char option;
    while ((option = getopt(argc, argv, "hc:s:i:")) != -1) {
        switch (option) {
            case 'c':
                strings_count = atoi(optarg);
                break;
            case 's':
                string_size = atoi(optarg);
                break;
            case 'i':
                iterations_count = atoi(optarg);
                break;
            case 'h':
                usage();
                exit(0);
            case '?':
                printf("Error: invalid argument");
                exit(1);
            default:
                exit(1);
        }
    }

	fd = open("/dev/lexsorter", O_RDWR);
	for (i = optind; i < argc; i++) {
		write(fd, argv[i], strlen(argv[i]));	
	}

	size = strings_count * string_size;
	char* strings = (char*) malloc(size * sizeof(char));
	for (i = 0; i < iterations_count; i++) {
		memset(strings, (int) '\0', size * sizeof(char));
		int bytes_read = read(fd, strings, size * sizeof(char));
		printf("Iteration: %d, bytes read:%d\nStrings:\n%s\n--END--\n", i + 1, bytes_read, strings);
	}
	free(strings);
}




