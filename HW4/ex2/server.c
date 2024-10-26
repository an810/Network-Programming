#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <libgen.h>

#define BUFF_SIZE 512

int openFile(char *output_dir, char *file_name, FILE **file, char *response)
{
    char *base_name = basename(file_name);

    char output_file_path[BUFF_SIZE];
    snprintf(output_file_path, BUFF_SIZE, "%s/%s", output_dir, base_name);
    if (access(output_file_path, F_OK) == 0)
    {
        fprintf(stderr, "Error: File already exists\n");
        strcpy(response, "ERROR: File already exists\n");
        return 1;
    }
    *file = fopen(output_file_path, "wb");
    if (!*file)
    {
        fprintf(stderr, "Error: Opening file\n");
        strcpy(response, "ERROR: Opening file\n");
        return 1;
    }
    return 0;
}
void writeFile(FILE *file, int connfd, long file_size)
{
    char buff[BUFF_SIZE + 1];
    int bytes_received, chunk_count = 0;
    long total_bytes_received = 0;
    while ((bytes_received = recv(connfd, buff, BUFF_SIZE, 0)) > 0)
    {
        ++chunk_count;
        printf("Chunk %d: %d bytes\n", chunk_count, bytes_received);
        fwrite(buff, 1, bytes_received, file);
        total_bytes_received += bytes_received;
        if (total_bytes_received >= file_size)
        {
            break;
        }
    }
}
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <Port>\n", argv[0]);
        return 1;
    }
    int serv_port = atoi(argv[1]);

    char output_dir[BUFF_SIZE];
    if (getcwd(output_dir, sizeof(output_dir)) == NULL)
    {
        fprintf(stderr, "Error: Getting current working directory\n");
        return 1;
    }
    printf("Current Directory: %s\n", output_dir);

    int listenfd, connfd;
    struct sockaddr_in servAddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(serv_port);
    if (bind(listenfd, (struct sockaddr *)&servAddr, sizeof(servAddr)))
    {
        fprintf(stderr, "Error: BINDING\n");
        return 1;
    }

    if (listen(listenfd, 10))
    {
        fprintf(stderr, "Error: LISTENING\n");
        return 1;
    }
    printf("Server Started");

    struct sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    while (1)
    {
        connfd = accept(listenfd, (struct sockaddr *)&clientAddr, (socklen_t *)&clientAddrLen);
        if (connfd < 0)
        {
            fprintf(stderr, "Error: Accepting connection\n");
            continue;
        }

        printf("Receving request from %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

        char file_name[BUFF_SIZE];
        if (recv(connfd, file_name, BUFF_SIZE, 0) <= 0)
        {
            fprintf(stderr, "Error: Receiving file name\n");
            close(connfd);
            continue;
        }
        FILE *file;
        char response[BUFF_SIZE];
        if (openFile(output_dir, file_name, &file, response) != 0)
        {
            if (send(connfd, response, strlen(response), 0) < 0)
            {
                fprintf(stderr, "Error: Sending response\n");
            }
            close(connfd);
            continue;
        }

        if (send(connfd, "OK", 2, 0) < 0)
        {
            fprintf(stderr, "Error: Sending response\n");
            fclose(file);
            close(connfd);
            continue;
        }

        long file_size;
        if (recv(connfd, &file_size, sizeof(file_size), 0) < 0)
        {
            fprintf(stderr, "Error: Receiving file size\n");
            fclose(file);
            close(connfd);
            continue;
        }
        printf("Receiving file: %s\n", file_name);
        printf("File size: %ld bytes\n", file_size);
        writeFile(file, connfd, file_size);
        fclose(file);
        close(connfd);
        printf("File received and saved to %s/%s\n", output_dir, file_name);
    }
}
