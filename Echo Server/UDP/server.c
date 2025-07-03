#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024


//For Daytime
// #include <time.h>
/**
time_t timer;
struct tm* tm_info;
char daytime_str[125]; 



char *daytime() {    // Get current time and format it
    time_t timer = time(NULL);
    struct tm *tm_info = localtime(&timer);

    strftime(daytime_str, sizeof(daytime_str), "%Y-%m-%d %H:%M:%S %Z", tm_info);
    return daytime_str;
}

**/



/**
For Math Expression Evaluation

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

const char *expr;  // Global pointer to current position in the expression

// Forward declarations
double parse_expr();

void skip_spaces() {
    while (isspace(*expr)) expr++;
}

double parse_number() {
    skip_spaces();
    double value = strtod(expr, (char**)&expr);
    return value;
}

double parse_factor() {
    skip_spaces();
    if (*expr == '(') {
        expr++;  // skip '('
        double val = parse_expr();
        skip_spaces();
        if (*expr == ')') expr++;  // skip ')'
        return val;
    } else {
        return parse_number();
    }
}

double parse_term() {
    double val = parse_factor();
    skip_spaces();
    while (*expr == '*' || *expr == '/') {
        char op = *expr++;
        double rhs = parse_factor();
        if (op == '*') val *= rhs;
        else if (op == '/') val /= rhs;
    }
    return val;
}

double parse_expr() {
    double val = parse_term();
    skip_spaces();
    while (*expr == '+' || *expr == '-') {
        char op = *expr++;
        double rhs = parse_term();
        if (op == '+') val += rhs;
        else if (op == '-') val -= rhs;
    }
    return val;
}

char *eval_math_expr(const char *input_expr) {
    static char result[64];
    expr = input_expr;
    double val = parse_expr();
    snprintf(result, sizeof(result), "%.10g", val);  // return as string
    return result;
}

 */




int main() {
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Fill server information
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("UDP server listening on port %d...\n", PORT);

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int n = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
        buffer[n] = '\0';
        printf("Client: %s\n", buffer);
        // Echo message back
        sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr *)&client_addr, addr_len);
    }



    close(sockfd);
    return 0;
}
