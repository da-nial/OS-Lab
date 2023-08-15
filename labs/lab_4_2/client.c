#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 4096

void receive(void *sock){
    int socket = *((int *) sock);

    char buffer[BUFFER_SIZE];
    int valread = read(socket, buffer, BUFFER_SIZE);

    while (valread > 0){
        buffer[valread] = '\0';
        printf("Message Received: %s\n", buffer);

        valread = read(socket, buffer, BUFFER_SIZE);
    }
 }


int main(int argc, char const *argv[]) {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;


    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    // sets all memory cells to zero
    memset(&serv_addr, '0', sizeof(serv_addr));

    // sets port and address family
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    // connects to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    printf("Established a connection with server...\n");

    pthread_t thread;
    pthread_create(&thread, NULL, receive, (void *) &sock);

    // Sending username to server
    printf("Sending Username to the server...\n");
    char username[BUFFER_SIZE];
    strcpy(username, argv[3]);
    username[strlen(argv[3])] = '\0';
    send(sock, username, BUFFER_SIZE, 0);
    
    printf("Enter Your Message (or Command)\n");


    char *buffer;
    size_t bufsize = BUFFER_SIZE;
    size_t characters;

    buffer = (char *)malloc(bufsize * sizeof(char));
    if( buffer == NULL)
    {
        perror("Unable to allocate buffer");
        exit(1);
    }

    while (strcmp("/quit", buffer)){
        printf("Type something: ");
        characters = getline(&buffer, &bufsize, stdin);

        // printf("%zu characters were read.\n",characters);

        buffer[strlen(buffer) - 1] = '\0';

        printf("Sending: %s\n", buffer);
        send(sock, buffer, BUFFER_SIZE, 0);

    }

    printf("Quiting...\n");
    pthread_cancel(thread);

    printf("Shutting down");
    shutdown(sock, 2);

    return 0;
 }
