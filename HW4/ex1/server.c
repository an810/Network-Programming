#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFF_SIZE 1024

int splitString(char *buffer, char *output) {
    char only_strings[100], only_numbers[100];
    strcpy(only_strings, buffer);
    int k = 0;
    strcpy(only_numbers, buffer);
    int j = 0;

    for (int i = 0; i < 100; i++) {
        char ch = only_numbers[i];
        if (ch == '\0') break;
        if (ch >= '0' && ch <= '9') {
            only_numbers[j] = ch;
            j++;
        } else if ((ch >= 'a' && ch <= 'z') || (ch == ' ')) {
            only_strings[k] = ch;
            k++;
        } else {
            return 0;
        }
    }
    only_numbers[j] = '\0';
    only_strings[k] = '\0';
    strcpy(output, only_numbers);
    strcat(output, "\n");
    strcat(output, only_strings);
    return 1;
}


int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Please input a port number.\n");
        return 1;
    }
    int serv_port = atoi(argv[1]);
    int listenfd, connfd;
    char buff[BUFF_SIZE + 1];
    struct sockaddr_in servAddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(serv_port);
    if (bind(listenfd, (struct sockaddr *)&servAddr, sizeof(servAddr)))
    {
        perror("Error: BINDING");
        return 1;
    }

    if (listen(listenfd, 10))
    {
        perror("Error: LISTENING");
        return 1;
    }
    printf("Server Started");

    struct sockaddr_in clientAddr;
    int rcvBytes, sendBytes, clientAddrLen = sizeof(clientAddr);
    while (1)
    {
        connfd = accept(listenfd, (struct sockaddr *)&clientAddr, (socklen_t *)&clientAddrLen);
        rcvBytes = recv(connfd, buff, BUFF_SIZE, 0);
        if (rcvBytes < 0)
        {
            perror("Error: Reading from client");
            return 1;
        }
        else
        {
            buff[rcvBytes] = '\0';
            printf("Receive from client[%s:%d] %s\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), buff);
            char response[BUFF_SIZE] = "\0";
            splitString(buff, response);
            sendBytes = send(connfd, response, strlen(response), 0);
            if (sendBytes < 0)
            {
                perror("Error: Sending to client");
                return 1;
            }
        }
        close(connfd);
    }
}