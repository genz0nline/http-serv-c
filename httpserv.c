#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <stdint.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>


#define BACKLOG     20

void initialize_hints(struct addrinfo *hints);
int get_bound_socket(struct addrinfo *servinfo);
void *get_in_addr(struct sockaddr *sa);

void start_server(char *port) {
    int status;
    char ipstr[INET6_ADDRSTRLEN];
    struct addrinfo hints;
    struct addrinfo *servinfo, *p;
    struct sockaddr_storage their_addr;
    socklen_t their_addr_size;
    int sockfd, accepted_fd;
    int buflen = 1024;
    char buf[buflen];

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
        their_addr_size = sizeof their_addr;
        accepted_fd = accept(sockfd, (struct sockaddr *) &their_addr, &their_addr_size);

        if (accepted_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(AF_INET, &their_addr, ipstr, sizeof(ipstr));
        printf("server: got connection from %s\n", ipstr);

        recv(accepted_fd, buf, buflen, 0);
        printf("received: %s\n", buf);

        close(accepted_fd);
    }
}

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    } else {
        return &(((struct sockaddr_in6 *)sa)->sin6_addr);
    }
}

int get_bound_socket(struct addrinfo *servinfo) {
    struct addrinfo *p;
    int yes = 1;
    int sockfd;

    for (p = servinfo; p != NULL; p = p->ai_next) {
        sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
        if (sockfd == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("setsockopt");
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
