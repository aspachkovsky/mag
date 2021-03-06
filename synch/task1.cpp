#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main (void) {

	int i;
	int p = 4;
 
	for (i = 0; i < p; i++) {
		printf("Process [pid = %d]  with i = %d is forking... \n", getpid(), i);
		int pid = fork();
  	
		if (pid == -1) {
  			printf("Fork failed!");
			return 1;
		} 
		
		if (pid == 0) {
			for (int j = 0; j < i; j++) {			
				printf("Child process [pid = %d] with i = %d says: Hello! \n", getpid(), i);
			}	
			return 0;
		}
	}

	return 0; 
}
