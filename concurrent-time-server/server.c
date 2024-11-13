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
#define BACKLOG 3

int main () {
    int sockfd,newsockfd;
    time_t curr_time;
    struct sockaddr_in address,client_address;
    int addrlen = sizeof(struct sockaddr_in);

    if((sockfd = socket(AF_INET,SOCK_STREAM,0))==-1) {
        perror("socket");
        exit(1);
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = INADDR_ANY;

    if((bind(sockfd,(struct sockaddr *)&address,addrlen))==-1) {
        perror("bind");
        exit(1);
    }

    if((listen(sockfd,BACKLOG))==-1) {
        perror("listen");
        exit(1);
    }

    if((newsockfd = accept(sockfd,(struct sockaddr *)&client_address,&addrlen))==-1) {
        perror("accept");
        exit(1);
    }

    time(&curr_time);
    char *timestr = ctime(&curr_time);
    send(newsockfd,timestr,strlen(timestr),0);
    printf("\nClient requested for Server time !");
    printf("\nCurrent time : %s sent to client",timestr);

    close(newsockfd);
    close(sockfd);
}