#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<netdb.h>

#define PORT 8090
#define BUFFER_SIZE 1024
#define IP "127.0.0.1"

int main() {
    int sockfd;
    struct sockaddr_in address;
    char buffer[BUFFER_SIZE] = {0};

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    if (inet_pton(AF_INET, IP, &address.sin_addr) <= 0) {
        perror("invalid address");
        close(sockfd);
        exit(1);
    }

    if (connect(sockfd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("connect");
        close(sockfd);
        exit(1);
    }

    char name[BUFFER_SIZE];
    printf("\nEnter the name to be displayed in chat server: ");
    fgets(name, BUFFER_SIZE, stdin);
    name[strcspn(name, "\n")] = 0;

    send(sockfd, name, strlen(name), 0); 
    printf("\nConnected to the chat server as %s\n", name);

    while (1) {
        printf("\nEnter message to send: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        send(sockfd, buffer, strlen(buffer), 0);
    }

    close(sockfd);
    return 0;
}
