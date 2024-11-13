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
    int sockfd,new_sockfd;
    struct sockaddr_in my_addr,client_addr;
    int addrlen = sizeof(struct sockaddr_in);
    char buffer[BUFFER_SIZE] = {0};

    if((sockfd = socket(AF_INET,SOCK_STREAM,0))==-1) {
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

    if(listen(sockfd,BACKLOG)==-1) {
        perror("listen");
        close(sockfd);
        exit(1);
    }

    printf("\nServer listening on port %d",PORT);

    if((new_sockfd = accept(sockfd,(struct sockaddr *)&client_addr,&addrlen))==-1) {
        perror("accept");
        close(sockfd);
        exit(1);
    }

    printf("\nServer : Got connection from %s\n",inet_ntoa(client_addr.sin_addr));

    while(1)
    {
        memset(buffer,0,BUFFER_SIZE);
        int valread = recv(new_sockfd,buffer,BUFFER_SIZE,0);
        if(valread==-1) {
            perror("recv");
            break;
        }
        else if(valread==0) {
            printf("Client disconnected !");
            break;
        }
        printf("\nClient : %s",buffer);

        printf("\nEnter message for client(type and enter exit to close the server) : ");
        fgets(buffer,BUFFER_SIZE,stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        if(strcmp(buffer,"exit")==0) {
            printf("\nShutting down the server...");
            break;
        }
        send(new_sockfd,buffer,BUFFER_SIZE,0);
    }
    

    close(new_sockfd);
    close(sockfd);
}