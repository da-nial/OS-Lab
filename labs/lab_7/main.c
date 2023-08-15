#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

/* These colors are used to print on console colorful, Readers use red, Writer use blue */
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_RESET "\x1b[0m"

#define NUMBER_OF_RESOURCES 5
/* this maybe any values >= 0 */
#define NUMBER_OF_CUSTOMERS 5
/* the available amount of each resource */
int available[NUMBER_OF_RESOURCES];
/* the maximum demand of each customer*/
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES] = {{0, 0, 1, 2, 5},
                                                         {1, 7, 8, 0, 8},
                                                         {5, 6, 5, 9, 3},
                                                         {0, 6, 5, 2, 2},
                                                         {7, 6, 5, 6, 7}};
/* the amount currently allocated to each customer */
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES] = {{0, 0, 1, 2, 2},
                                                            {1, 0, 3, 0, 5},
                                                            {2, 2, 5, 5, 1},
                                                            {0, 6, 3, 2, 1},
                                                            {1, 0, 3, 4, 3}};
;
/* the remaining need of each customer */
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
/* mutex lock to prevent customers changing resources at the same time, i.e. prevent race conditions */
pthread_mutex_t mutex;

void *customer(void *customer_number);
int request_resources(int customer_num, int request[]);
int release_resources(int customer_num, int release[]);
int is_safe();
void generate_request(int customer_num, int request_vector[]);
void generate_release(int customer_num, int release_vector[]);

int main(int argc, char **argv)
{
    // available = {4, 8, 2, 5, 3};

    if (argc < 6)
    {
        printf("not enough arguments\n");
        return EXIT_FAILURE;
    }
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
    {
        available[i] = strtol(argv[i + 1], NULL, 10);
    }
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
    {
        printf("av[%d]: %d\n", i, available[i]);
    }

    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
    {
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++)
        {
            need[i][j] = maximum[i][j] - allocation[i][j];
        }
    }

    pthread_mutex_init(&mutex, NULL);

    pthread_t t_id[5];

    pthread_attr_t t_attr;
    pthread_attr_init(&t_attr);

    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
    {
        pthread_create(&t_id[i], &t_attr, customer, (void *)&i);
    }

    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
    {
        pthread_join(t_id[i], NULL);
    }

    return 0;
}

void *customer(void *customer_number)
{
    int customer_num = *((int *)customer_number);

    int request_vector[NUMBER_OF_RESOURCES] = {0};
    int release_vector[NUMBER_OF_RESOURCES] = {0};

    while (1)
    {
        sleep(1);
        pthread_mutex_lock(&mutex);
        generate_request(customer_num, request_vector);
        request_resources(customer_num, request_vector);
        pthread_mutex_unlock(&mutex);

        sleep(1);
        pthread_mutex_lock(&mutex);
        generate_release(customer_num, release_vector);
        release_resources(customer_num, release_vector);
        pthread_mutex_unlock(&mutex);
    }
}

int is_safe()
{
    int finish[NUMBER_OF_CUSTOMERS] = {0}; // 0 for not finished, 1 for finished.
    int work[NUMBER_OF_RESOURCES];
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
    {
        work[i] = available[i]; // take a copy of available
    }

    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
    {
        if (finish[i])
        {
            continue;
        }
        else
        {
            int is_resources_enough = 1; // 1 for enough, 0 for not enough
            for (int j = 0; j < NUMBER_OF_RESOURCES; j++)
            {
                if (need[i][j] < work[j])
                {
                }
                else // Not enough resources to allocate,
                {
                    is_resources_enough = 0;
                }
            }
            if (is_resources_enough)
            {
                finish[i] = 1;
                for (int k = 0; k < NUMBER_OF_RESOURCES; k++)
                {
                    work[k] += allocation[i][k];
                    i = -1; // start over
                    break;
                }
            }
            else
            {
                continue;
            }
        }
    }

    int safety = 1;
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
    {
        if (!finish[i])
        {
            safety = 0;
            break;
        }
    }
    return safety;
}

int request_resources(int customer_num, int request[])
{
    printf("\nCustomer %d requested [", customer_num);
    for (int i = 0; i < NUMBER_OF_RESOURCES - 1; i++)
    {
        printf("%d, ", request[i]);
    }
    printf("%d]\n", request[NUMBER_OF_RESOURCES - 1]);

    /* Check to see if the request is safe */
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
    {
        if (request[i] > need[customer_num][i])
        {
            printf("\x1b[31m\tbut they only need %d of %d, request rejected.\n\x1b[0m",
                   need[customer_num][i], i);
            return -1;
        }
        else if (request[i] > available[i])
        {
            printf("\x1b[31m\tbut available resources of type %d is %d, request rejected.\n\x1b[0m", i, available[i]);
            return -1;
        }
    }

    printf("\x1b[34mrequest accepted.\n\x1b[0m");

    for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
    {
        available[i] -= request[i];
        allocation[customer_num][i] += request[i];
        need[customer_num][i] -= request[i];
    }
    if (is_safe())
    {
        printf("\x1b[34m\tStill safe, proceeding...\n\x1b[0m");
        return 0;
    }
    else
    {
        printf("\x1b[31m\tNot safe anymore, rolling back...\n\x1b[0m");
        for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
        {
            available[i] += request[i];
            allocation[customer_num][i] -= request[i];
            need[customer_num][i] += request[i];
        }
        return -1;
    }
}

int release_resources(int customer_num, int release[])
{
    printf("\nCustomer %d wants to release [", customer_num);
    for (int i = 0; i < NUMBER_OF_RESOURCES - 1; i++)
    {
        printf("%d, ", release[i]);
    }
    printf("%d]\n", release[NUMBER_OF_RESOURCES - 1]);

    for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
    {
        if (release[i] > allocation[customer_num][i])
        {
            printf("\x1b[31m\tbut they only have %d items of type %d, release failed\n\x1b[0m", allocation[customer_num][i], i);
            return -1;
        }
    }
    printf("\x1b[34m\tReleased successfuly\n\x1b[0m");
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
    {
        available[i] += release[i];
        allocation[customer_num][i] -= release[i];
        need[customer_num][i] += release[i];
    }
    return 0;
}

void generate_request(int customer_num, int request_vector[])
{
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
    {
        if (need[customer_num][i] != 0)
        {
            request_vector[i] = rand() % (need[customer_num][i] + 1);
        }
        else
        {
            request_vector[i] = 0;
        }
    }
}

void generate_release(int customer_num, int release_vector[])
{
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
    {
        if (allocation[customer_num][i] != 0)
        {
            release_vector[i] = rand() % (allocation[customer_num][i] + 1);
        }
        else
        {
            release_vector[i] = 0;
        }
    }
}