//
// Created by Gabriel Mitterrutzner on 12.04.17.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

//thread
#include <pthread.h>
#include <sys/wait.h>
#include <sys/time.h>

#define MAXLENGTH 1024

char message[MAXLENGTH];

void* threadOperation(int *arg){
    int socket = *arg;
    int serverMessageLength;
    char serverMessage[MAXLENGTH];

    while ((serverMessageLength = (int) recv(socket , serverMessage , 2000 , 0)) > 0) {
        if (strcmp(serverMessage, "q") != 0) {
            printf("%s\n", serverMessage);
        } else {
            printf("heii");
            break;
        }
    }

    free(arg);
    pthread_exit(NULL);
}

int main(int argc, const char * argv[]) {
    pthread_t thread;
    struct sockaddr_in serverAdress;
    serverAdress.sin_family = AF_INET;
    serverAdress.sin_port = htons(5000);
    serverAdress.sin_addr.s_addr = htonl(INADDR_ANY);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1) {
        perror("socket()");
        return 1;
    }

    printf("Warten auf Server\n");

    if (connect(sockfd, (const struct sockaddr *) &serverAdress, sizeof(serverAdress)) == -1) {
        perror("connect()");
        return 2;
    }

    int* momentsock = malloc(1);
    *momentsock = sockfd;

    pthread_create(&thread, NULL, (void*(*)(void*)) &threadOperation, momentsock);

    char mess[MAXLENGTH];

    do{
        printf("Sende den Text: ");
        scanf("%[^\n]s", mess);
        getc(stdin);
        if (send(sockfd, mess, sizeof(mess), 0) < 0) {
            perror("send()");
            return 3;
        }
    }while (strcmp(mess, "q"));

    pthread_join(thread, NULL);

    close(sockfd);

    return 0;
}