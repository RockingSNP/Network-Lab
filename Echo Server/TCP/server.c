#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#define PORT 8080


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
    int server_fd, new_socket;
    char buffer[1024];
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
        write(new_socket, buffer, n); // Echo back
    }

    close(new_socket);
    close(server_fd);
    return 0;
}
