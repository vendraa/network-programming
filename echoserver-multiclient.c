#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

void handle_client(int connfd, struct sockaddr_in cliaddr) {
    char buffer[1024];
    int n;

    printf("Client connected from %s:%d (PID = %d)\n",
           inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port), getpid());

    while ((n = read(connfd, buffer, sizeof(buffer)-1)) > 0) {
        buffer[n] = '\0';
        printf("[PID %d] Received: %s", getpid(), buffer);
        write(connfd, buffer, n);
    }

    printf("[PID %d] Client disconnected.\n", getpid());
    close(connfd);
    exit(0);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <Port>\n", argv[0]);
        exit(1);
    }

    int listenfd, connfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t clilen;
    int port = atoi(argv[1]);

    signal(SIGCHLD, SIG_IGN);

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        perror("Socket Error");
        exit(1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    if (bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("Bind Error");
        exit(1);
    }

    if (listen(listenfd, 5) < 0) {
        perror("Listen Error");
        exit(1);
    }

    printf("Echo server running on port %d (PID = %d)...\n", port, getpid());

    for (;;) {
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);

        if (connfd < 0) {
            perror("Accept Error");
            continue;
        }

        pid_t pid = fork();
        if (pid == 0) { 
            close(listenfd);
            handle_client(connfd, cliaddr);
        } else if (pid > 0) {
            close(connfd);
        } else {
            perror("Fork Error");
            close(connfd);
        }
    }
}
