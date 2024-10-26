#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define BUFF_SIZE 1024

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Please input IP address and port number\n");
        return 0;
    }
    const char *serv_addr = argv[1];
    const int serv_port = atoi(argv[2]);
    int clientfd;
    char buff[BUFF_SIZE + 1];
    struct sockaddr_in servAddr;

    clientfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(serv_addr);
    servAddr.sin_port = htons(serv_port);

    if (connect(clientfd, (struct sockaddr *)&servAddr, sizeof(servAddr))) {
        perror("Error: CONNECT");
        return 1;
    }

    int ret;
    printf("Send to server: ");
    fgets(buff, BUFF_SIZE, stdin);
    buff[strlen(buff) - 1] = '\0';
    ret = send(clientfd, buff, strlen(buff), 0);
    if (ret < 0) {
        perror("Error: Sending to server");
        return 1;
    }
    ret = recv(clientfd, buff, BUFF_SIZE, 0);
    if (ret < 0) {
        perror("Error: Receiving from server");
        return 1;
    } else {
        buff[ret] = '\0';
        printf("Received from server: \n%s\n", buff);
    }
    close(clientfd);
    return 0;
}