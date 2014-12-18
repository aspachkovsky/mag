#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/sem.h>

#define shm_size 1024

struct sembuf sem_buf_wakeup = {0, 1, 0};
struct sembuf sem_buf_wait = {0, -1, 0};
 
int sem_id;
char* shmem;
int shm_key = 1234;

#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
/* union semun is defined by including <sys/sem.h> */
#else
/* according to X/OPEN we have to define it ourselves */
union semun {
      int val;                  /* value for SETVAL */
      struct semid_ds *buf;     /* buffer for IPC_STAT, IPC_SET */
      unsigned short *array;    /* array for GETALL, SETALL */
                                /* Linux specific part: */
      struct seminfo *__buf;    /* buffer for IPC_INFO */
};
#endif
 
void client(char* message) {
	
	while (1) {
		semop(sem_id, &sem_buf_wait, 1);
		
		if (strlen(shmem) == 0) {
			strcpy(shmem, message);
		}
		
		semop(sem_id, &sem_buf_wakeup, 1);
		sleep(1);
	}
}

 
void server() {
	
	union semun semopts;
	semopts.val = 1;
	semctl(sem_id, 0, SETVAL, semopts);

	memset(shmem, 0, shm_size);

	while (1) {
		semop(sem_id, &sem_buf_wait, 1);
		if (strlen(shmem) > 0) {
			printf("Server received: %s \n", shmem);
			memset(shmem, 0, shm_size);
		}		
		semop(sem_id, &sem_buf_wakeup, 1);
	}
}

int main(int argc, char* argv[]) {
	
	int shm_id = shmget(shm_key, shm_size, IPC_CREAT | 0666); // create
 	if (shm_id < 0) {
		printf("shget failed! \n");
		return 1;
	}
 
	shmem = (char *) shmat(shm_id, 0, 0); // attach
	if (shmem == (char *) -1) {
		printf("shmat failed! \n");
		return 1;
	}
	
	int sem_key = 9999;
	sem_id = semget(sem_key, 1, IPC_CREAT | 0660);
	if (sem_id == -1) {
		printf("semget failed!");
		return 1;
	} 
	
	if (argc == 1) {
		printf("Launched as Server. Listening to clients... \n");
		server();
	} else if (argc == 2) {
		printf("Launched as Client. Sending messages to the server... \n");
		client(argv[1]);
	} else {
		printf("Wrong number of arguments. \n");
		return 1;
	}
	
	if(shmdt(shmem) != 0) {
		printf("shmdt failed!\n");
	} 
	
	return 0;
}
