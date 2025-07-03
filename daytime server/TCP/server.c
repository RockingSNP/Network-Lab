#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <time.h>

#define PORT 8080
#define BUFFER_SIZE 1024

time_t timer;
struct tm* tm_info;
char daytime_str[125]; 



char *daytime() {    // Get current time and format it
    time_t timer = time(NULL);
    struct tm *tm_info = localtime(&timer);

    strftime(daytime_str, sizeof(daytime_str), "%Y-%m-%d %H:%M:%S %Z", tm_info);
    return daytime_str;
}

int main() {
    int server_fd, new_socket;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in server_addr;
    socklen_t addrlen = sizeof(server_addr);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_fd, 5);

    new_socket = accept(server_fd, (struct sockaddr*)&server_addr, &addrlen);

    while (1) {
        int n = read(new_socket, buffer, sizeof(buffer));
        if (n <= 0) break;
        write(new_socket, daytime(), strlen(daytime())); // Echo back
    }

    close(new_socket);
    close(server_fd);
    return 0;
}
