#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdint.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define BACKLOG     20

void initialize_hints(struct addrinfo *hints);
int get_bound_socket(struct addrinfo *servinfo);

void start_server(char *port) {
    int status;
    char ipstr[INET6_ADDRSTRLEN];
    struct addrinfo hints;
    struct addrinfo *servinfo, *p;
    int sockfd;

    initialize_hints(&hints);

    status = getaddrinfo(NULL, port, &hints, &servinfo);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    sockfd = get_bound_socket(servinfo);
    freeaddrinfo(servinfo);

    if (sockfd == -1) {
        fprintf(stderr, "httpserv: falied to bind socket\n");
        exit(2);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(3);
    }

    while(1) {
        
    }

}

int get_bound_socket(struct addrinfo *servinfo) {
    struct addrinfo *p;
    int sockfd;

    for (p = servinfo; p != NULL; p = p->ai_next) {
        sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
        if (sockfd == -1) {
            perror("server: socket");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) != 0) {
            perror("server: bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
        return -1;
    }

    return sockfd;
}

void initialize_hints(struct addrinfo *hints) {
    memset(hints, 0, sizeof(*hints));
    hints->ai_family = AF_UNSPEC;
    hints->ai_socktype = SOCK_STREAM;
    hints->ai_flags = AI_PASSIVE;
}

int main(void) {
    start_server("42069");
}
