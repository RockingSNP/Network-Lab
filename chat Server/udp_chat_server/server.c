// udp_server_minimal.c
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 20
#define BUF 1000
#define USR 50

struct Client {
    char name[USR];
    struct sockaddr_in addr;
    int active;
} clients[MAX_CLIENTS];

int find_by_name(const char *n) {
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (clients[i].active && strcmp(clients[i].name, n) == 0)
            return i;
    return -1;
}

int find_by_addr(struct sockaddr_in *a) {
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (clients[i].active &&
            clients[i].addr.sin_addr.s_addr == a->sin_addr.s_addr &&
            clients[i].addr.sin_port == a->sin_port)
            return i;
    return -1;
}

int main() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in serv = {.sin_family = AF_INET, .sin_port = htons(9999), .sin_addr.s_addr = INADDR_ANY};
    bind(sock, (void *)&serv, sizeof(serv));

    char buf[BUF];
    struct sockaddr_in cli;
    socklen_t len = sizeof(cli);

    while (1) {
        int n = recvfrom(sock, buf, BUF - 1, 0, (void *)&cli, &len);
        buf[n] = 0;

        int idx = find_by_addr(&cli);

        if (idx == -1) {
            for (int i = 0; i < MAX_CLIENTS; i++)
                if (!clients[i].active) {
                    strcpy(clients[i].name, buf);
                    clients[i].addr = cli;
                    clients[i].active = 1;
                    break;
                }
            continue;
        }

        if (strncmp(buf, "/chat:", 6) == 0) {
            char *to = strtok(buf + 6, ":");
            char *msg = strtok(NULL, "");
            int tgt = find_by_name(to);
            if (to && msg && tgt != -1) {
                char out[BUF];
                snprintf(out, BUF, "[%s -> %s]: %s\n", clients[idx].name, to, msg);
                sendto(sock, out, strlen(out), 0, (void *)&clients[tgt].addr, sizeof(clients[tgt].addr));
            }
        } else {
            char out[BUF];
            snprintf(out, BUF, "[%s]: %s", clients[idx].name, buf);
            for (int i = 0; i < MAX_CLIENTS; i++)
                if (clients[i].active && i != idx)
                    sendto(sock, out, strlen(out), 0, (void *)&clients[i].addr, sizeof(clients[i].addr));
        }
    }

    return 0;
}
