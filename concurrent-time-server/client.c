#include<stdio.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>
#include<time.h>
/*The above imports are actually enough in latest C distributions as all other necessary modules
are linked within arpa/inet.h . But chances are if you are using an older distribution you might
have to include these additional modules too*/
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<netdb.h>

#define PORT 8080
#define BUFFERSIZE 1024
#define IP "127.0.0.1"

int main() {
    int sockfd;
    struct sockaddr_in server_address;
    int addrlen = sizeof(struct sockaddr_in);
    char buffer[BUFFERSIZE];

    if((sockfd = socket(AF_INET,SOCK_STREAM,0))==-1) {
        perror("socket");
        exit(1);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    if(inet_pton(AF_INET,IP,&server_address.sin_addr)==-1) {
        perror("address");
        exit(1);
    }

    if(connect(sockfd,(struct sockaddr *)&server_address,addrlen)==-1) {
        perror("connect");
        exit(1);
    }

    printf("\nConnected to Server !");

    int valread = recv(sockfd,buffer,BUFFERSIZE,0);
    if(valread==-1) {
        perror("recv");
        exit(1);
    }
    if(valread==0) {
        printf("\nServer is down ! Connection terminated !");
        exit(1);
    }

    printf("\nTime request sent to server !");
    printf("\nServer time : %s",buffer);

    close(sockfd);
}