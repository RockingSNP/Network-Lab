#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int s = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in a = {AF_INET, htons(9000)};
    inet_pton(AF_INET, "127.0.0.1", &a.sin_addr);
    connect(s, (struct sockaddr*)&a, sizeof(a));

    char b[1024], f[256];
    fgets(f, sizeof(f), stdin);
    f[strcspn(f, "\n")] = 0;
    send(s, f, strlen(f), 0);

    FILE *fp = fopen("recv", "wb");
    int n;
    while ((n = recv(s, b, sizeof(b), 0)) > 0) fwrite(b, 1, n, fp);

    fclose(fp); close(s);
}
