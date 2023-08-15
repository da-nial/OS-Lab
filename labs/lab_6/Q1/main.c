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

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_RESET "\x1b[0m"

#define BUF_SIZE 1024
#define SHM_KEY 0x1235
#define MAX_COUNT 200

int _read()
{
    int shmid = shmget(SHM_KEY, BUF_SIZE, 0444 | IPC_CREAT);

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
        printf("\x1b[31m Reader Process, with PID %d, Count: %d\n\x1b[0m", getpid(), *count);
        // usleep(1);
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
    int shmid = shmget(SHM_KEY, BUF_SIZE, 0644 | IPC_CREAT);
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

    for (*count = 0; *count < MAX_COUNT; (*count)++)
    {
        printf("\x1b[34mWriter Process, with PID %d, Count: %d\n\x1b[0m", getpid(), *count);
        // usleep(1);
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
    if (fork() == 0)
        _read();
    else
    {
        if (fork() == 0)
            _read();
        else
            _write();
    }

    return 0;
}
