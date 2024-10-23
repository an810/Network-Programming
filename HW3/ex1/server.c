#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define BUFF_SIZE 1024

int split(char *buffer, char *output) {
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

    int sockfd, rcvBytes, sendBytes;
    socklen_t len;
    char buff[BUFF_SIZE+1];
    struct sockaddr_in servaddr, cliaddr;
    int port = atoi(argv[1]);

    char output[100];

    //Step 1: Construct socket
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        perror("Error: ");
        return 0;
    }

    //Step 2: Bind address to socket
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);
    if(bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))){perror("Error: ");
        return 0;
    }
    printf("Server started.");

    // Step 3: Communicate with client
    for (;;)
    {
        len = sizeof(cliaddr);
        rcvBytes = recvfrom(sockfd, buff, BUFF_SIZE, 0,
                            (struct sockaddr *)&cliaddr, &len);
        buff[rcvBytes] = '\0';
        printf("[%s:%d]: %s\n", inet_ntoa(cliaddr.sin_addr),
               ntohs(cliaddr.sin_port), buff);

        split(buff, output);
        char result[200];
        sendto(sockfd, output, strlen(output), 0, (struct sockaddr *)&cliaddr, len);
    }
    return 0;
}