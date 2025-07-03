#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1000

int sock;
struct sockaddr_in serv_addr;

void *receive_thread(void *arg) {
    char buf[BUFFER_SIZE];
    struct sockaddr_in from;
    socklen_t len = sizeof(from);

    while (1) {
        int n = recvfrom(sock, buf, BUFFER_SIZE - 1, 0, (struct sockaddr *)&from, &len);
        if (n > 0) {
            buf[n] = 0;
            printf("%s", buf);
            fflush(stdout);
        }
    }
    return NULL;
}

int main() {
    sock = socket(AF_INET, SOCK_DGRAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(9999);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    char username[100];
    printf("Enter username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0;  // remove newline

    sendto(sock, username, strlen(username), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    pthread_t t;
    pthread_create(&t, NULL, receive_thread, NULL);

    char msg[BUFFER_SIZE];
    while (fgets(msg, sizeof(msg), stdin)) {
        sendto(sock, msg, strlen(msg), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        if (strcmp(msg, "/exit\n") == 0) break;
    }

    close(sock);
    return 0;
}
