#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 9000
#define BUFFER_SIZE 1024

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_fd, 5);

    printf("Server listening on port %d...\n", PORT);

    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
    if (client_fd < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    // Receive filename from client
    int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    buffer[bytes_received] = '\0';
    printf("Client requested file: %s\n", buffer);

    FILE *fp = fopen(buffer, "rb");
    if (fp == NULL) {
        strcpy(buffer, "ERROR: File not found.");
        send(client_fd, buffer, strlen(buffer), 0);
    } else {
        while (!feof(fp)) {
            int bytes_read = fread(buffer, 1, sizeof(buffer), fp);
            if (bytes_read > 0) {
                send(client_fd, buffer, bytes_read, 0);
            }
        }
        printf("File sent successfully.\n");
        fclose(fp);
    }

    close(client_fd);
    close(server_fd);
    return 0;
}