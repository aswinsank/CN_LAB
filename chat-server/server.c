#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
/*The above imports are actually enough in latest C distributions as all other necessary modules
are linked within arpa/inet.h . But chances are if you are using an older distribution you might
have to include these additional modules too*/
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<netdb.h>

#define PORT 8090
#define BACKLOG 10
#define BUFFERSIZE 1024

int main() {
    int sockfd, newsockfd;
    struct sockaddr_in address, client_address;
    int addrlen = sizeof(struct sockaddr_in);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&address, addrlen) == -1) {
        perror("bind");
        close(sockfd);
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        close(sockfd);
        exit(1);
    }

    printf("\nChat Server Active! Listening for incoming client connections on port %d!\n", PORT);

    while (1) {
        if ((newsockfd = accept(sockfd, (struct sockaddr *)&client_address, &addrlen)) == -1) {
            perror("accept");
            close(sockfd);
            exit(1);
        }
        char name[BUFFERSIZE];
        int valread = recv(newsockfd, name, BUFFERSIZE, 0);
        if (valread > 0) {
            name[valread] = '\0'; 
            printf("\n%s has joined the server!\n", name);
            fflush(stdout);
        } else {
            perror("recv name");
            close(newsockfd);
            exit(1);
        }
        if (fork() == 0) {
            close(sockfd);
            while (1) {
                char buffer[BUFFERSIZE];
                valread = recv(newsockfd, buffer, BUFFERSIZE, 0);
                if (valread > 0) {
                    buffer[valread] = '\0';
                    printf("\n%s: %s", name, buffer);
                } else if (valread == 0) {
                    printf("\n%s has left the chat!\n", name);
                    break;
                } else {
                    perror("recv");
                    break;
                }
            }
            close(newsockfd);
            exit(0);
        } else {
            close(newsockfd);
        }
    }

    close(sockfd);
    return 0;
}
