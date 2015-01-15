#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void usage(void) {
	printf("Usage: test [-c strings_count] [-s string_size] strings... \n");
	printf("-c    strings_count  the number of strings to be read (10 by default), optional.\n");
	printf("-s    string_size    a size of a string in bytes (256 by default), optional.\n");
	printf("-h    prints this message.\n");
}

int main(int argc, char** argv) {
	int i, fd, strings_count = 10, string_size = 256;

	char option;
    while ((option = getopt(argc, argv, "hc:s:")) != -1) {
        switch (option) {
            case 'c':
                strings_count = atoi(optarg);
                break;
            case 's':
                string_size = atoi(optarg);
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
	
	char* strings = (char*) malloc(10 * 256 * sizeof(char));
	int bytes_read = read(fd, strings, 256);
	printf("Bytes read:%d\nStrings:\n%s\n--END--\n", bytes_read, strings);
	free(strings);
}




