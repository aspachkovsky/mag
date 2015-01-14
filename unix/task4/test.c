#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


/*
struct storage {
	char** strings;
	unsigned int length;
};	

#define DIM 10

void add_to_storage(struct storage* st, char* str) {

	char* in = str;

	int i = 0;
	for (; i < st->length; i++) {
		if (strcmp(in, st->strings[i]) < 0) {
			char* temp = st->strings[i];
			st->strings[i] = in;
			in = temp;
		}
	}

	if (st->length < DIM) {
		st->strings[st->length++] = in;
	}
}
*/
int main(int argc, char** argv) {
	
	int fd = open("/dev/lexsorter", O_RDWR);

	printf("%d\n", argc);
	int i = 1;
	for (; i < argc; i++) {
		write(fd, argv[i], strlen(argv[i]));	
	}
	
	char* k = (char*) malloc(256);
	int a = read(fd, k, 256);
	printf("Read:%d\n%s\n--END--\n", a, k);
	
/*
	struct storage* sp = NULL;
	sp = (struct storage*) malloc(sizeof(*sp));
	sp->length = 0;
	sp->strings = (char**) malloc(DIM * sizeof(char*));

	add_to_storage(sp, "bbb");
	add_to_storage(sp, "ccc");
	add_to_storage(sp, "aaa");
	add_to_storage(sp, "ddd");

	int i = 0;
	for (; i < sp->length; i++){
        printf("%d - %s\n", i+1, sp->strings[i]);
    }
*/


	

}




