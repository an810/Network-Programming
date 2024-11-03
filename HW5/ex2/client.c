// client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <libgen.h>

#define BUFFER_SIZE 1024
#define SERVER_IP "127.0.0.1"
#define PORT 5501

int main() {
    int sock_fd;
    struct sockaddr_in server_addr;
    char filepath[256];
    char buffer[BUFFER_SIZE];
    FILE *input_file, *output_file;
    size_t total_bytes_sent = 0;
    
    // Create socket
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    
    // Connect to server
    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Failed to connect to server");
        exit(EXIT_FAILURE);
    }
    
    printf("Connected to server\n");
    
    // Get input file path from user
    printf("Enter the path to the input text file: ");
    fgets(filepath, sizeof(filepath), stdin);
    filepath[strcspn(filepath, "\n")] = 0;
    
    // Open input file
    input_file = fopen(filepath, "r");
    if (input_file == NULL) {
        perror("Failed to open input file");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    // Create output filename
    char *input_name = basename(filepath);
    char output_path[300];
    snprintf(output_path, sizeof(output_path), "capitalized_%s", input_name);
    
    // Send file contents
    while (!feof(input_file)) {
        size_t bytes_read = fread(buffer, 1, BUFFER_SIZE, input_file);
        if (bytes_read > 0) {
            ssize_t bytes_sent = send(sock_fd, buffer, bytes_read, 0);
            if (bytes_sent == -1) {
                perror("Failed to send data");
                break;
            }
            total_bytes_sent += bytes_sent;
        }
    }

    // Send end of file marker
    sleep(1);  // Small delay to ensure last buffer is sent
    send(sock_fd, "END_OF_FILE", 11, 0);
    
    // Open output file
    output_file = fopen(output_path, "w");
    if (output_file == NULL) {
        perror("Failed to create output file");
        fclose(input_file);
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    // Receive and save capitalized file
    while (1) {
        ssize_t bytes_received = recv(sock_fd, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) break;
        
        if (bytes_received >= 11 && strncmp(buffer, "END_OF_FILE", 11) == 0) break;
        
        fwrite(buffer, 1, bytes_received, output_file);
    }
    
    printf("\nTotal bytes sent: %zu\n", total_bytes_sent);
    printf("Capitalized file saved as: %s\n", output_path);
    
    fclose(input_file);
    fclose(output_file);
    close(sock_fd);
    return 0;
}