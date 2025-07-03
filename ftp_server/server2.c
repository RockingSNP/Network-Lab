#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int s = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {AF_INET, htons(9000), INADDR_ANY};
    bind(s, (struct sockaddr*)&addr, sizeof(addr));
    listen(s, 1);

    int c = accept(s, NULL, NULL);
    char buf[1024];
    int r = recv(c, buf, sizeof(buf)-1, 0);
    buf[r] = 0;
    FILE *f = fopen(buf, "rb");

    if (!f) send(c, "File not found", 14, 0);
    else while ((r = fread(buf, 1, sizeof(buf), f)) > 0) send(c, buf, r, 0);

    fclose(f); close(c); close(s);
}
