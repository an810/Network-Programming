#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>

#define BUFF_SIZE 1024

int isIPAddressValid(const char* str) {
    struct sockaddr_in sa;
    return inet_pton(AF_INET, str, &(sa.sin_addr)) != 0;
}

int isIPAddress(const char* str) {
    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == '.') {
            continue;
        }
        if (str[i] < '0' || str[i] > '9') {
            return 0;
        }
    }
    return 1;
}


void printHostNameFromAddr(const char* strAddr, char *output) {
    struct in_addr addr;
    if(!isIPAddressValid(strAddr)) {
        strcat(output, "Invalid address\n");
        return;
    }
    if (inet_pton(AF_INET, strAddr, &addr) == 1) {
        struct hostent* host = gethostbyaddr(&addr, sizeof(addr), AF_INET);
        if (host) {
            strcat(output, "Official domain name: ");
            strcat(output, host->h_name);
            strcat(output, "\n");
            for (int i = 0; host->h_aliases[i]; i++) {
                if (i == 0) {
                    strcat(output, "Alias domain name:");
                }
                strcat(output, " ");
                strcat(output, host->h_aliases[i]);
            }
            strcat(output, "\n");
        } else {
            strcat(output, "Not found information\n");
        }
    } else {
        strcat(output, "Invalid address\n");
    }
}

void printAddrFromHostName(const char* hostName, char *output) {
    struct hostent* host = gethostbyname(hostName);
    if (host) {
        strcat(output, "Official name: ");
        strcat(output, host->h_name);
        strcat(output, "\n");
        for (int i = 0; host->h_aliases[i]; i++) {
            if (i == 0) {
                strcat(output, "Alias name:");
            }
            strcat(output, " ");
            strcat(output, host->h_aliases[i]);
        }
        strcat(output, "\n");

        strcat(output, "Official IP: ");
        strcat(output, inet_ntoa(*(struct in_addr*)host->h_addr_list[0]));
        strcat(output, "\n");
        for (int i = 1; host->h_addr_list[i]; i++) {
            if (i == 1) {
                strcat(output, "Alias IP:");
            }
            strcat(output, " ");
            strcat(output, inet_ntoa(*(struct in_addr*)host->h_addr_list[i]));
        }
        strcat(output, "\n");
    } else {
        strcat(output, "Not found information\n");
    }
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
    memset(&servaddr, 0, sizeof(servaddr));
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
        // clear output
        memset(output, 0, sizeof(output));
        if (isIPAddress(buff))
        {
            printHostNameFromAddr(buff, output);
        }
        else
        {
            printAddrFromHostName(buff, output);
        }
        char result[200];
        sendto(sockfd, output, strlen(output), 0, (struct sockaddr *)&cliaddr, len);
        if (sendBytes < 0) {
            perror("Error: ");
            continue;
        }
    }
    return 0;
}