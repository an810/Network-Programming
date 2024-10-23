#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define BUFF_SIZE 1024

int main(int argc, char *argv[])
{
    if (argc != 3) // Ensure the user provides the host and port as arguments
    {
        printf("Please provide an IP address and a port number.\n");
        return 1;
    }

    int sockfd, rcvBytes, sendBytes;
    socklen_t len;
    char buff[BUFF_SIZE + 1];
    struct sockaddr_in servaddr;
    
    char *server_address = argv[1];     // Server address from command line
    int port = atoi(argv[2]);           // Convert the port argument to an integer

    // Step 1: Construct socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Error: ");
        return 0;
    }

    // Step 2: Define the address of the server
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    if (inet_pton(AF_INET, server_address, &servaddr.sin_addr) <= 0)
    {
        perror("Error: Invalid address/Address not supported");
        return 0;
    }
    servaddr.sin_port = htons(port); // Use the port provided by the user

    // Step 3: Communicate with the server
    for (;;)
    {
        printf("Send to server: ");
        fgets(buff, BUFF_SIZE, stdin);

        // Remove trailing newline character, if any
        buff[strlen(buff) - 1] = '\0';

        // Exit the loop if input is empty or "***" is entered
        if (strlen(buff) == 0 || strcmp(buff, "***") == 0)
        {
            printf("Exiting...\n");
            break;
        }

        len = sizeof(servaddr);
        sendBytes = sendto(sockfd, buff, strlen(buff), 0, (struct sockaddr *)&servaddr, len);
        if (sendBytes < 0) {
            perror("Error: ");
            return 1;
        }

        rcvBytes = recvfrom(sockfd, buff, BUFF_SIZE, 0, (struct sockaddr *)&servaddr, &len);
        if (rcvBytes < 0) {
            perror("Error: ");
            return 1;
        }       

        buff[rcvBytes] = '\0'; // Null-terminate the received string
        printf("Reply from server: \n%s\n", buff);
    }

    // Close the socket
    close(sockfd);

    return 0;
}