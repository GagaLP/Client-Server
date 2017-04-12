//
// Created by Gabriel Mitterrutzner on 12.04.17.
//

//normal
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>


//thread
#include <pthread.h>
#include <sys/wait.h>
#include <sys/time.h>

#define clear() printf("\033[H\033[J");

void* threadOperation(int *arg){
    int socket = *arg;
    long clientMessageLength;
    char clientMessage[1024];

    while ((clientMessageLength = recv(socket, clientMessage , 2000 , 0)) > 0) {
        if (strcmp(clientMessage, "q")) {
            printf("%s\n", clientMessage);
        }
    }

    printf("Client hat sich abgemeldet\n");

    free(arg);
    pthread_exit(NULL);
}

int main(int argc, const char * argv[]) {
    int i = 0;
    struct sockaddr_in my_addr;
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(5000);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int clientSokets = 0;

    printf("Current time: %s",__TIME__);

    pthread_t threads[2];

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1) {
        perror("socket()");
        return 1;
    }

    if (bind(sockfd, (const struct sockaddr *) &my_addr, sizeof(my_addr)) == -1) {
        perror("bind()");
        return 2;
    }

    printf("Warte auf Clients\n");
    listen(sockfd, 2);

    socklen_t sin_size = sizeof(struct sockaddr_in);
    struct sockaddr_in remote_host;

    while (i < 2) {
        clientSokets = accept(sockfd, (struct sockaddr *) &remote_host, &sin_size);
        int* new_sock = malloc(1);
        *new_sock = clientSokets;

        if (clientSokets == -1) {
            perror("accept()");
            return 3;
        }

        printf("Client %d ist da\n", i);

        pthread_create(&threads[i], NULL, (void*(*)(void*)) &threadOperation, new_sock);
        i++;
    }

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
    close(clientSokets);
    close(sockfd);
    return 0;
}