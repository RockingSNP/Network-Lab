#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define BUF 1024
int sockfd;

void *recv_thread(void *arg) {
    char buf[BUF];
    while (1) {
        int n = recv(sockfd, buf, BUF - 1, 0);
        if (n <= 0) exit(0);
        buf[n] = 0;
        printf("%s", buf);
        fflush(stdout);
    }
}

int main(int c, char **v) {
    if (c != 3) return printf("Usage: %s <ip> <port>\n", v[0]), 1;

    struct sockaddr_in s = {.sin_family = AF_INET, .sin_port = htons(atoi(v[2]))};
    inet_pton(AF_INET, v[1], &s.sin_addr);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(sockfd, (void *)&s, sizeof(s))) return perror("connect"), 1;

    char buf[BUF];
    recv(sockfd, buf, BUF - 1, 0);  // receive prompt
    printf("%s", buf);
    fgets(buf, BUF, stdin);        // send username
    send(sockfd, buf, strlen(buf), 0);

    pthread_t t;
    pthread_create(&t, 0, recv_thread, 0);
    while (fgets(buf, BUF, stdin))
        send(sockfd, buf, strlen(buf), 0);
    pthread_cancel(t);
    close(sockfd);
    return 0;
}
