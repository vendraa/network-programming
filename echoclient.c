#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFSIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <ServerIP> <Port>\n", argv[0]);
        exit(1);
    }

    int sockfd;
    struct sockaddr_in servaddr;
    char sendline[BUFSIZE], recvline[BUFSIZE];
    int port = atoi(argv[2]);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);

    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
        perror("Invalid address");
        exit(1);
    }

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Connect failed");
        exit(1);
    }

    printf("Connected to echo server %s:%d\n", argv[1], port);
    printf("Ketik pesan kamu kemudian tekan Enter (Ctrl+D untuk keluar):\n");

    while (fgets(sendline, BUFSIZE, stdin) != NULL) {
        write(sockfd, sendline, strlen(sendline));
        int n = read(sockfd, recvline, BUFSIZE - 1);
        if (n > 0) {
            recvline[n] = '\0';
            printf("Echo: %s", recvline);
        }
    }

    close(sockfd);
    return 0;
}
