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

#define clear() printf("\033[H\033[J")

#define MAXLENGTH 1024

struct clientStruct{
    int sock;
    int sequence;
    char message[MAXLENGTH];
}clientStruct1;

void increaseSequenceNumber(){
    if (clientStruct1.sequence != INT8_MAX){
        clientStruct1.sequence ++;
    } else {
        clientStruct1.sequence = 0;
    }
}

void* listenSocket(int *arg){
    int socket = *arg;
    long clientMessageLength;
    char clientMessage[MAXLENGTH];

    while ((clientMessageLength = recv(socket, clientMessage , 2000 , 0)) > 0) {
        clear();
        clientStruct1.sock = socket;
        strcpy(clientStruct1.message, clientMessage);
        increaseSequenceNumber();
    }

    free(arg);
    pthread_exit(NULL);
}

void* writeSocket(int *arg){
    int socket = *arg;
    int last = 0;
    while (1){
        if (clientStruct1.sequence != last) {
            if (clientStruct1.sock == socket) {
                if(strcmp(clientStruct1.message, "q") == 0){
                    if(send(socket, "q", sizeof("q"), 0) < 0) {
                        perror("send()");
                    }
                    break;
                }
            } else {
                if(send(socket, clientStruct1.message, sizeof(clientStruct1.message), 0) < 0) {
                    perror("send()");
                }
            }
            last = clientStruct1.sequence;
        }
    }

    printf("Client hat sich abgemeldet\n");
    pthread_exit(NULL);
}

int main(int argc, const char * argv[]) {
    int i = 0;
    struct sockaddr_in my_addr;
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(5000);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int clientSokets = 0;

    printf("Current time: %s\n",__TIME__);

    pthread_t listenThreads[2];
    pthread_t writeThreads[2];

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

        pthread_create(&listenThreads[i], NULL, (void*(*)(void*)) &listenSocket, new_sock);
        pthread_create(&writeThreads[i], NULL, (void*(*)(void*)) &writeSocket, new_sock);
        i++;
    }

    for (int j = 0; j < 2; ++j) {
        pthread_join(listenThreads[j], NULL);
        pthread_join(writeThreads[j], NULL);
    }

    close(clientSokets);
    close(sockfd);
    return 0;
}