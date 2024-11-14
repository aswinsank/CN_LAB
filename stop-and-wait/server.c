#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#define BUFFERSIZE 1024
#define PORT 4002
#define TOTALFRAMES 10
#define LOSSPROB 0.2

typedef struct
{
    int seqNo;
    char buffer[BUFFERSIZE];
} Frame;

int main()
{
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    Frame dataFrame, ackFrame;
    int framesize = sizeof(Frame);

    srand(time(NULL));

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&server_addr, addrlen) == -1)
    {
        perror("bind");
        exit(1);
    }

    int expectedSeqNo = 0;

    while (expectedSeqNo < TOTALFRAMES)
    {
        int valread = recvfrom(sockfd, &dataFrame, framesize, 0, (struct sockaddr *)&client_addr, &addrlen);
        if (valread > 0)
        {
            if (dataFrame.seqNo == expectedSeqNo)
            {
                printf("Frame-%d received: %s\n", expectedSeqNo, dataFrame.buffer);
                ackFrame.seqNo = expectedSeqNo;
                if ((rand() % 100) / 100.0 > LOSSPROB)
                {
                    sendto(sockfd, &ackFrame, framesize, 0, (struct sockaddr *)&client_addr, addrlen);
                    printf("ACK sent for Frame-%d\n", expectedSeqNo);
                }
                else
                {
                    printf("Simulating ACK loss: ACK for Frame-%d not sent!\n", expectedSeqNo);
                }
                expectedSeqNo++;
            }
            else
            {
                printf("Out of Order Frame-%d received and discarded\n", dataFrame.seqNo);
                ackFrame.seqNo = expectedSeqNo - 1;
                sendto(sockfd, &ackFrame, framesize, 0, (struct sockaddr *)&client_addr, addrlen);
                printf("Resending the last valid ACK\n");
            }
        }
        else
        {
            perror("recvfrom");
            exit(1);
        }
    }
    printf("All Frames received successfully!\n");
    close(sockfd);
    return 0;
}