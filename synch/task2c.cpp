#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define shm_size 1024

struct sembuf sem_buf_wakeup = {0, 1, 0};
struct sembuf sem_buf_wait = {0, -1, 0};
 
int main(int argc, char* argv[]) {
	char c, tmp;
	key_t shm_key = 1234;
	char *s;	

 	int shm_id = shmget(shm_key, shm_size, IPC_CREAT | 0666); // create
 	if (shm_id < 0) {
		printf("Client: shget failed!");
		return 1;
	}
 
	char *shmem = (char *) shmat(shm_id, NULL, 0); // attach
	if (shmem == (char *) -1) {
		printf("Client: shmat failed!");
		return 1;
	}

	int sem_key = 9999;
	int sem_id = semget(sem_key, 1, IPC_CREAT | 0660);

	if (sem_id == -1) {
		printf("Client: semget failed!");
		return 1;
	} 

	memset(shmem, 0, shm_size);

	while (1) {
		semop(sem_id, &sem_buf_wait, 1);

		strcpy(shmem, argv[1]);
		
		semop(sem_id, &sem_buf_wakeup, 1);
		sleep(1);
	}
 
	if(shmdt(shmem) != 0) {
		printf("Client: shmdt failed!\n");
	}

	return 0;
}
