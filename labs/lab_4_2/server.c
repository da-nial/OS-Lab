#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdbool.h>


#define PORT 8080
#define BUFFER_SIZE 4096
#define MAX_CLIENTS 100


char* concat(const char *s1, const char *s2) {
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

int num_clients = 0;

struct client{
	int id;
    char username[BUFFER_SIZE];

	int socket;
    struct sockaddr_in address;

    int groups_joined[100];
	int num_groups_joined;

    bool isOnline;
};

struct client clients[MAX_CLIENTS];
pthread_t serverSockets[MAX_CLIENTS];

void *serverSocket(void *client_p){    
    int valread;
    char buffer[BUFFER_SIZE];

    struct client* client = (struct client*) client_p;
    int id = (*client).id;
    int socket = (*client).socket;

    valread = read(socket, buffer, BUFFER_SIZE);
    buffer[valread] = '\0';

    (*client).isOnline = true;
    (*client).num_groups_joined = 0;
    strcpy((*client).username, buffer);

    while (strcmp("/quit", buffer)){
        int valread = read(socket, buffer, BUFFER_SIZE);

        if (valread < 0){
            continue;
        }

		buffer[valread] = '\0';

        printf("\n[RAW MESSAGE FROM %s WITH SIZE %d]: %s\n", (*client).username, valread, buffer);
        
        // To get group id
        char msg_cpy[BUFFER_SIZE];
        strcpy(msg_cpy, buffer);
        char *_ = strtok(msg_cpy, " ");
        char *group_id_string = strtok(NULL, " ");
        int group_id = atoi(group_id_string);

        bool isMemberOf = false;
        int group_id_index;

        for(int i = 0; i < (*client).num_groups_joined; i++){
				if((*client).groups_joined[i] == group_id)
					isMemberOf = true;  
                    group_id_index = i;
                    break;
        }

        if (!(strncmp("join", buffer, 4))) {
            printf("[CLIENT %s WANTS TO JOIN GROUP %d]\n", (*client).username, group_id);
            if (!isMemberOf){
                (*client).groups_joined[(*client).num_groups_joined++] = group_id;
				printf("[CLIENT %s SUCCESSFULLY JOINED GROUP %d]\n" , (*client).username, group_id);
            }
            else
            {
                printf("[NOTHING HAPPENED, CLIENT %s WAS ALREADY IN THE GROUP %d]\n", (*client).username, group_id);
            }
        }

        else if (!(strncmp("send", buffer, 4))) {
            char content[BUFFER_SIZE];
            strcpy(content, buffer + 6 + strlen(group_id_string));

            char *sender = malloc(BUFFER_SIZE);
            sender = concat((*client).username, ": ");

            char *message = malloc(BUFFER_SIZE);
            message = concat(sender, content);

            printf("[CLIENT %s WANTS TO SEND MESSAGE `%s` TO GROUP %d]\n", (*client).username, message, group_id);

            if (isMemberOf){
                send(socket, message, BUFFER_SIZE, 0);
                for(int i = 0; i < num_clients; i++)
                    for(int j = 0; j < (clients[i]).num_groups_joined; j++)
                        if((clients[i]).groups_joined[j] == group_id && (clients[i]).isOnline && clients[i].id != (*client).id)
                            send((clients[i]).socket, message, BUFFER_SIZE, 0);
                printf("[MESSAGE SENT SUCCESSFULLY]\n");
            }
            else{
                printf("[CLIENT %s WAS NOT A MEMBER OF GROUP %d, MESSAGE DID NOT SEND]\n", (*client).username, group_id);
            }

            free(sender);
            free(message);
        }

        else if (!(strncmp("leave", buffer, 5))){
            printf("[CLIENT %s WANTS TO LEAVE GROUP %d]\n", (*client).username, group_id);
            if (isMemberOf){
                (*client).num_groups_joined--;
                (*client).groups_joined[group_id_index] = (*clients).groups_joined[(*client).num_groups_joined];
                // clients[id].groupId[clients[id].num_groups_joined] = -1;
                printf("[CLIENT %s SUCCESSFULLY LEFT GROUP %d]\n" , (*client).username, group_id);
            }
            else{
                printf("[NOTHING HAPPENED, CLIENT %s WAS NOT IN THE GROUP %d]\n", (*client).username, group_id);
            }
        }
    }

    printf("[Client %s LOST THEIR CONNECTION, TURNING THEIR STATUS TO OFFLINE\n", (*clients).username);
    (*client).isOnline = false;
    pthread_cancel(serverSockets[id]);
    shutdown((*client).socket , 2);

    return 0;
}

int main(int argc, char const *argv[]) {
    char buffer[1024];

    // creates socket file descriptor
    int server_fd;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(atoi(argv[1])); // host to network -- coverts the ending of the given integer
    const int addrlen = sizeof(address);

    // binding
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // listening on server socket with backlog size 3.
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Listening on %s:%d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

    while(1){
        // accepting client
        // accept returns client socket and fills given address and addrlen with client address information.
        if (
            (clients[num_clients].socket = accept(server_fd, (struct sockaddr*) &clients[num_clients].address, (socklen_t *) &addrlen)) < 0){
            perror("accept");
            exit(EXIT_FAILURE);
        }
        clients[num_clients].id = num_clients;

        printf("Hello client %s:%d\n", inet_ntoa(clients[num_clients].address.sin_addr),
                                       ntohs(clients[num_clients].address.sin_port));

		pthread_create(&serverSockets[num_clients], NULL, serverSocket, (void *) &clients[num_clients]);
		num_clients ++;
	}
    
    printf("Shutting down server");
    return 0;
}