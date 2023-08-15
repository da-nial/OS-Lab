#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define NO_OF_PHILOSOPHERS 5

pthread_t philosophers[NO_OF_PHILOSOPHERS];
pthread_mutex_t mutex_chopsticks[NO_OF_PHILOSOPHERS]; // Used to prevent two philosphers take or give up chopsticks at the same time.
int chopsticks[NO_OF_PHILOSOPHERS] = {0};             // 0 to show the chopstick is free, 1 to shows it is used.

void philosopher(int i)
{
    int right = i;    // Right chopstick index is always the same as philosopher's index
    int left = i - 1; // Left Chopstick index is always the same as philospher's index - 1, except for the last philosopher.
    if (left == -1)
        left = NO_OF_PHILOSOPHERS - 1;
    int locked;
    while (1)
    {
        locked = 0;
        while (!locked)
        {
            pthread_mutex_lock(&mutex_chopsticks[left]);
            pthread_mutex_lock(&mutex_chopsticks[right]);
            if (chopsticks[right] || chopsticks[left])
            {
                pthread_mutex_unlock(&mutex_chopsticks[left]); // give up the chopsticks unless you can take both at once.
                pthread_mutex_unlock(&mutex_chopsticks[right]);
                printf("\x1b[31mPhilospher %d tried to eat, but failed. One of the chopsticks %d or %d is currently used.\n\x1b[0m", i, left, right);
                usleep(random() % 1000); // think.
                continue;
            }
            chopsticks[right] = 1; // take chopsticks.
            chopsticks[left] = 1;

            pthread_mutex_unlock(&mutex_chopsticks[left]);
            pthread_mutex_unlock(&mutex_chopsticks[right]);
            locked = 1;
        }

        printf("\x1b[34mPhilosopher %d is eating using chopstick[%d] and chopstick[%d]\n\x1b[34m", i, left, right);
        usleep(random() % 500);
        printf("Philosopher %d finished eating\n", i);
        printf("Philospher %d is thinking\n", i);
        pthread_mutex_lock(&mutex_chopsticks[left]); // give up chopsticks.
        pthread_mutex_lock(&mutex_chopsticks[right]);
        chopsticks[right] = 0;
        chopsticks[left] = 0;
        pthread_mutex_unlock(&mutex_chopsticks[left]);
        pthread_mutex_unlock(&mutex_chopsticks[right]);
        usleep(random() % 1000);
    }
}

int main()
{
    for (int i = 0; i < NO_OF_PHILOSOPHERS; i++)
        pthread_mutex_init(&mutex_chopsticks[i], NULL);

    for (int i = 0; i < NO_OF_PHILOSOPHERS; i++)
        pthread_create(&philosophers[i], NULL, philosopher, (void *)i);

    for (int i = 0; i < NO_OF_PHILOSOPHERS; i++)
        pthread_join(philosophers[i], NULL);
    return 0;
}