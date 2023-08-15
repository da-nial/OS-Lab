#include<stdio.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

#define BUF_SIZE 1024
#define SHM_KEY 0x1234


int main(int argc, char *argv[]) {
   int shmid;

   shmid = shmget(SHM_KEY, BUF_SIZE, 0644|IPC_CREAT);
   if (shmid == -1) {
      perror("Shared memory");
      return 1;
   }

   // Attach to the segment to get a pointer to it.
   char *str = (char*) shmat(shmid,(void*)0,0);

   if (str == (void *) -1) {
      perror("Shared memory attach");
      return 1;
   }

   printf("Enter Your Sentence!\n");
   fgets(str,100, stdin);
   sprintf(str + strlen(str), "(Written By Process %ld)\n\n", (long) getpid());

   printf("Writing the sentence on memory address %d\n", SHM_KEY);

   printf("Process %d: Finished Writing!\n", getpid());


   printf("Detaching Shared Memory\n");
   if (shmdt(str) == -1) {
      perror("shmdt");
      return 1;
   }

   printf("Terminating Process\n");
   return 0;
}
