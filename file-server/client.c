#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<netdb.h>

#define PORT 8080
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

    if (inet_pton(AF_INET, IP, &address.sin_addr) == -1) {
        perror("invalid address");
        exit(1);
    }

    if (connect(sockfd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("connect");
        close(sockfd);
        exit(1);
    }

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        printf("\nEnter file name (type exit to quit): ");
        fgets(buffer, BUFFER_SIZE, stdin);

        buffer[strcspn(buffer, "\n")] = 0;

        if (strcmp(buffer, "exit") == 0) {
            printf("\nConnection terminated !");
            break;
        }

        send(sockfd, buffer, strlen(buffer), 0);

        memset(buffer, 0, BUFFER_SIZE);

        int valread;
        FILE *file;
        file = fopen("retrieved.txt","a");
        while ((valread = recv(sockfd, buffer, BUFFER_SIZE, 0)) > 0) {
            if (strncmp(buffer, "file not found", strlen("file not found")) == 0) {
                printf("\nFile not found!\n");
                break;
            }
            fprintf(file,"%s",buffer);
            printf("%s", buffer);  
            memset(buffer, 0, BUFFER_SIZE);  
        }
        fclose(file);
        printf("\nFile retrieved succesfully ! File saved as retrieved.txt !");
        if (valread <= 0) {
            printf("\nConnection terminated !\n");
            break;
        }
    }

    close(sockfd);
    return 0;
}
