#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>

/* These colors are used to print on console colorful, Readers use red, Writer use blue */
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_RESET "\x1b[0m"

#define SEM_NAME "MUTEX"       // SEMAPHORE USED TO PREVENT MUTUAL ACCESS OF COUNT VARIABLE
#define RC_SEM_NAME "RC_MUTEX" /* SEMAPHORE USED TO PREVENT MUTUAL ACCESS FOR UPDATING RC,              \
                                RC IS THE NUMBER OF READERS THAT ARE CURRENTLY READING COUNT VARIABLE.  \
                                THIS SEMAPHORE IS USED TO ALLOW SIMULTANOUS ACCESS OF COUNT BY READERS. \
                                 */
#define COUNT_SHM_KEY 0x1233   // COUNT VALUE ADDRESS IN MEMORY, WHICH IS SHARED BETWEEN READERS AND WRITER
#define RC_SHM_KEY 0x1234      // RC VALUE ADDRESS (SHARED), RC SHOWS THE NUMBER OF READERS CURRENTLY ACCESSING COUNT VARIABLE,
#define BUF_SIZE 1024
#define MAX_COUNT 200

int _read(int *rc)
{
    int shmid = shmget(COUNT_SHM_KEY, BUF_SIZE, 0444 | IPC_CREAT);
    sem_t *sem = sem_open(SEM_NAME, 0);       /* Open a preexisting semaphore. */
    sem_t *rc_sem = sem_open(RC_SEM_NAME, 0); /* Open a preexisting semaphore. */

    if (shmid == -1)
    {
        perror("Shared memory");
        return 1;
    }

    // Attach to the segment to get a pointer to it.
    int *count = (int *)shmat(shmid, (void *)0, 0);

    if (count == (void *)-1)
    {
        perror("Shared memory attach");
        return 1;
    }

    while (*count < MAX_COUNT)
    {
        // printf("first rc: %d", *rc);
        sem_wait(rc_sem);
        (*rc)++;
        if (*rc == 1)
            sem_wait(sem);
        sem_post(rc_sem);

        printf("\x1b[31m\tReader Process, with PID %d, Count: %d\n\x1b[0m", getpid(), *count);

        sem_wait(rc_sem);
        (*rc)--;
        if (*rc == 0)
            sem_post(sem);
        sem_post(rc_sem);
        printf("");
    }

    printf("Detaching Shared Memory\n");
    if (shmdt(count) == -1)
    {
        perror("shmdt");
        return 1;
    }

    if (shmctl(shmid, IPC_RMID, 0) == -1)
    {
        perror("shmctl");
        return 1;
    }
    return 0;
}

int _write()
{
    int shmid = shmget(COUNT_SHM_KEY, BUF_SIZE, 0644 | IPC_CREAT);
    sem_t *sem = sem_open(SEM_NAME, 0); /* Open a preexisting semaphore. */

    if (shmid == -1)
    {
        perror("Shared memory");
        return 1;
    }

    // Attach to the segment to get a pointer to it.
    int *count = (int *)shmat(shmid, (void *)0, 0);
    *count = 0;
    // usleep(50);

    if (count == (void *)-1)
    {
        perror("Shared memory attach");
        return 1;
    }

    while (*count < MAX_COUNT)
    {
        sem_wait(sem);
        printf("\x1b[34mWriter Process, with PID %d, Count: %d\n\x1b[0m", getpid(), ++(*count));
        // usleep(1);
        sem_post(sem);
    }

    printf("Detaching Shared Memory\n");
    if (shmdt(count) == -1)
    {
        perror("shmdt");
        return 1;
    }

    printf("Terminating Process\n");
    return 0;
}

int main(int argc, char *argv[])
{
    sem_t *sem = sem_open(SEM_NAME, O_CREAT, 0644, 1);       /* Initial value is 1. */
    sem_t *rc_sem = sem_open(RC_SEM_NAME, O_CREAT, 0644, 1); /* Initial value is 1. */

    int rc_shmid = shmget(RC_SHM_KEY, BUF_SIZE, 0644 | IPC_CREAT);
    int *rc = (int *)shmat(rc_shmid, (void *)0, 0); // Read Count
    *rc = 0;

    if (fork() == 0)
        _read(rc);
    else
    {
        if (fork() == 0)
            _read(rc);
        else
            _write();
    }

    return 0;
}
