// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <signal.h>

#define BUFFER_SIZE 1024
#define PORT 5501

void handle_sigchld(int sig) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void capitalize_text(char *text, size_t size) {
    for(size_t i = 0; i < size; i++) {
        text[i] = toupper(text[i]);
    }
}

void handle_client(int client_fd) {
    char buffer[BUFFER_SIZE];
    char temp_filename[] = "/tmp/serverXXXXXX";
    ssize_t bytes_received;
    FILE *temp_file;
    
    // Create temporary file
    int temp_fd = mkstemp(temp_filename);
    if (temp_fd == -1) {
        perror("Failed to create temporary file");
        return;
    }
    temp_file = fdopen(temp_fd, "w+");
    if (temp_file == NULL) {
        perror("Failed to open temporary file");
        close(temp_fd);
        unlink(temp_filename);
        return;
    }

    // Receive file content and write to temporary file
    while ((bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0)) > 0) {
        if (strncmp(buffer, "END_OF_FILE", 11) == 0) break;
        capitalize_text(buffer, bytes_received);
        fwrite(buffer, 1, bytes_received, temp_file);
    }
    fflush(temp_file);
    rewind(temp_file);

    // Send the capitalized file back
    while (!feof(temp_file)) {
        size_t bytes_read = fread(buffer, 1, BUFFER_SIZE, temp_file);
        if (bytes_read > 0) {
            send(client_fd, buffer, bytes_read, 0);
        }
    }

    // Send end of file marker
    sleep(1);  // Small delay to ensure last buffer is sent
    send(client_fd, "END_OF_FILE", 11, 0);

    // Cleanup
    fclose(temp_file);
    unlink(temp_filename);
    close(client_fd);
    exit(EXIT_SUCCESS);
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    pid_t child_pid;

    // Setup signal handler
    struct sigaction sa;
    sa.sa_handler = handle_sigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    
    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    
    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    // Bind socket
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    
    // Listen for connections
    if (listen(server_fd, SOMAXCONN) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    
    printf("Server listening on port %d...\n", PORT);
    
    while(1) {
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd == -1) {
            perror("Accept failed");
            continue;
        }
        
        printf("New client connected\n");

        child_pid = fork();
        
        if (child_pid == -1) {
            perror("Fork failed");
            close(client_fd);
            continue;
        }
        
        if (child_pid == 0) {  // Child process
            close(server_fd);
            handle_client(client_fd);
        } else {  // Parent process
            close(client_fd);
        }
    }
    
    close(server_fd);
    return 0;
}