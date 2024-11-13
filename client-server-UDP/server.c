#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
/*The above imports are actually enough in latest C distributions as all other necessary modules
are linked within arpa/inet.h . But chances are if you are using an older distribution you might
have to include these additional modules too*/
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<netdb.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define BACKLOG 3

int main() {
    int sockfd;
    int addrlen = sizeof(struct sockaddr_in);
    struct sockaddr_in my_addr,client_addr;
    char buffer[BUFFER_SIZE] = {0};

    if((sockfd = socket(AF_INET,SOCK_DGRAM,0))==-1) {
        perror("socket");
        exit(1);
    }

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(PORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;

    if((bind(sockfd,(struct sockaddr *)&my_addr,addrlen))==-1) {
        perror("bind");
        exit(1);
    }


    while(1)
    {
        memset(buffer,0,BUFFER_SIZE);
        int valread = recvfrom(sockfd,buffer,BUFFER_SIZE,0,(struct sockaddr *)&client_addr,&addrlen);
        if(valread==-1) {
            perror("recvfrom");
            break;
        }
        printf("\nClient : %s",buffer);

        memset(buffer,0,BUFFER_SIZE);
        printf("\nEnter message for client(type and enter exit to close the server) : ");
        fgets(buffer,BUFFER_SIZE,stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        if(strcmp(buffer,"exit")==0) {
            printf("\nShutting down the server !...");
            break;
        }
        sendto(sockfd,buffer,BUFFER_SIZE,0,(struct sockaddr *)&client_addr,addrlen);
    }
    
    close(sockfd);
}