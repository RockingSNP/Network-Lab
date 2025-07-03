#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_CLIENTS 20
#define BUFFER_SIZE 1024
#define MAX_USERNAME_LENGTH 50

struct Client {
    int fd;
    int in_use;
    char username[MAX_USERNAME_LENGTH];
} clients[MAX_CLIENTS];

int find_client_by_username(const char *username) {
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (clients[i].in_use && strcmp(clients[i].username, username) == 0)
            return clients[i].fd;
    return -1;
}

void send_private(int sender_fd, const char *sender_name, const char *target, const char *msg) {
    int target_fd = find_client_by_username(target);
    char buffer[BUFFER_SIZE];
    if (target_fd != -1) {
        snprintf(buffer, sizeof(buffer), "[%s -> %s] %s\n", sender_name, target, msg);
        send(target_fd, buffer, strlen(buffer), 0);
        printf("Private message from %s to %s: %s", sender_name, target, msg);
    } else {
        snprintf(buffer, sizeof(buffer), "User '%s' not found.\n", target);
        send(sender_fd, buffer, strlen(buffer), 0);
        printf("Private message failed: User '%s' not found.\n", target);
    }
}

void broadcast(int sender_fd, const char *sender_name, const char *msg) {
    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "[%s]: %s\n", sender_name, msg);
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (clients[i].in_use && clients[i].fd != sender_fd)
            send(clients[i].fd, buffer, strlen(buffer), 0);
            printf("Broadcast from %s: %s", sender_name, msg);
}

void *handle_client(void *arg) {
    int fd = *(int *)arg;
    free(arg);
    char name[MAX_USERNAME_LENGTH] = {0}, buf[BUFFER_SIZE];

    send(fd, "Enter username: ", 16, 0);
    int len = recv(fd, name, sizeof(name) - 1, 0);
    if (len <= 0) return close(fd), NULL;
    if (name[len - 1] == '\n') name[len - 1] = 0;

    int index = -1;
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (!clients[i].in_use) {
            clients[i] = (struct Client){fd, 1, ""};
            strncpy(clients[i].username, name, MAX_USERNAME_LENGTH);
            index = i;
            break;
        }
    if (index == -1) return send(fd, "Server full.\n", 13, 0), close(fd), NULL;

    while ((len = recv(fd, buf, sizeof(buf) - 1, 0)) > 0) {
        buf[len] = 0;
        if (strncmp(buf, "/chat:", 6) == 0) {
            char *target = strtok(buf + 6, ":");
            char *msg = strtok(NULL, "");
            if (target && msg)
                send_private(fd, name, target, msg);
        } else {
            broadcast(fd, name, buf);
        }
    }

    clients[index].in_use = 0;
    close(fd);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) return printf("Usage: %s <port>\n", argv[0]), 1;

    int port = atoi(argv[1]), server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv = {.sin_family = AF_INET, .sin_addr.s_addr = INADDR_ANY, .sin_port = htons(port)};
    bind(server_fd, (struct sockaddr *)&serv, sizeof(serv));
    listen(server_fd, 5);
    printf("Server running on port %d...\n", port);

    while (1) {
        struct sockaddr_in cli;
        socklen_t len = sizeof(cli);
        int *client_fd = malloc(sizeof(int));
        *client_fd = accept(server_fd, (struct sockaddr *)&cli, &len);
        if (*client_fd < 0) { free(client_fd); continue; }
        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, client_fd);
        pthread_detach(tid);
    }
    close(server_fd);
}
