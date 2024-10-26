#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <libgen.h>

#define BUFF_SIZE 512

int getFileSize(char* filepath) {
    struct stat file_stat;
    if (stat(filepath, &file_stat) < 0)
    {
        perror("Error: Getting file size");
        return -1;
    }
    return file_stat.st_size;
}

int sendingFileInfo(int clientfd, char *base_name, long file_size)
{
    if (send(clientfd, base_name, strlen(base_name), 0) < 0)
    {
        perror("Error: Sending file info");
        close(clientfd);
        return 1;
    }

    char response[BUFF_SIZE + 1] = "\0";
    if (recv(clientfd, response, BUFF_SIZE, 0) < 0)
    {
        perror("Error: Receiving response");
        close(clientfd);
        return 1;
    }

    if (strcmp(response, "OK") != 0)
    {
        printf("Server response: %s\n", response);
        close(clientfd);
        return 1;
    }

    if (send(clientfd, &file_size, sizeof(file_size), 0) < 0)
    {
        perror("Error: Sending file info");
        close(clientfd);
        return 1;
    }
    return 0;

}
int sendingFileData(int clientfd, FILE *file, long file_size)
{
    char buff[BUFF_SIZE + 1];
    int bytes_read;
    long total_bytes_read = 0;
    while ((bytes_read = fread(buff, 1, BUFF_SIZE, file)) > 0)
    {
        if (send(clientfd, buff, bytes_read, 0) < 0)
        {
            perror("Error: Sending file data");
            close(clientfd);
            return 1;
        }
        total_bytes_read += bytes_read;
        if (total_bytes_read >= file_size)
        {
            break;
        }
    }
    return 0;
}

void fileInput(FILE **file, char *base_name, long *file_size)
{
    char filepath[256];
    printf("Enter file path: ");
    fgets(filepath, 256, stdin);
    filepath[strlen(filepath) - 1] = '\0';

    strcpy(base_name, basename(filepath));
    *file_size = getFileSize(filepath);
    if (*file_size < 0)
    {
        return;
    }

    printf("Sending file %s, size %ld\n", base_name, *file_size);
    *file = fopen(filepath, "rb");
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <IP Address> <Port>\n", argv[0]);
        return 1;
    }

    char base_name[256];
    long file_size;
    FILE *file;
    fileInput(&file, base_name, &file_size);
    if (file_size < 0 || file == NULL)
    {
        return 1;
    }

    const char *serv_addr = argv[1];
    const int serv_port = atoi(argv[2]);
    int clientfd;
    struct sockaddr_in servAddr;

    clientfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(serv_addr);
    servAddr.sin_port = htons(serv_port);

    if (connect(clientfd, (struct sockaddr *)&servAddr, sizeof(servAddr)))
    {
        perror("Error: CONNECT");
        return 1;
    }

    
    if (sendingFileInfo(clientfd, base_name, file_size) != 0)
    {
        return 1;
    }

    if (sendingFileData(clientfd, file, file_size) != 0)
    {
        return 1;
    }

    
    fclose(file);
    close(clientfd);
    printf("File sent\n");
    return 0;
}