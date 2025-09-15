#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <Server IP> <Port>\n", argv[0]);
        exit(1);
    }

    int sockfd, n;
    char sendline[1024], recvline[1024];
    struct sockaddr_in servaddr;
    int port = atoi(argv[2]);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket Error");
        exit(1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);

    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
        perror("Invalid address");
        exit(1);
    }

    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("Connect Error");
        exit(1);
    }

    // dapatkan port lokal yang dipakai client
    struct sockaddr_in localaddr;
    socklen_t addrlen = sizeof(localaddr);
    getsockname(sockfd, (struct sockaddr *)&localaddr, &addrlen);

    printf("Client started (PID = %d), using local port %d\n", getpid(), ntohs(localaddr.sin_port));
    printf("Connected to server %s:%d\n", argv[1], port);

    while (fgets(sendline, sizeof(sendline), stdin) != NULL) {
        write(sockfd, sendline, strlen(sendline));
        if ((n = read(sockfd, recvline, sizeof(recvline)-1)) > 0) {
            recvline[n] = '\0';
            printf("Echo from server: %s", recvline);
        }
    }

    close(sockfd);
    return 0;
}
