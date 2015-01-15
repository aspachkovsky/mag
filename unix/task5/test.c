#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
	if (argc > 1) {
		int i;
		for (i = 1; i < argc; i++) {
			void *a = malloc(atoi(argv[i]));
			if (a == NULL) {
				printf("FAILED: malloc(%d)\n", atoi(argv[i]));
			}
			free(a);
			printf("\n");
		}
	}
}




