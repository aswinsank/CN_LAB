#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFERSIZE 1024
#define PORT 4004
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
        close(sockfd);
        exit(1);
    }

    int expectedSeqNo = 0;

    while (expectedSeqNo < TOTALFRAMES)
    {
        int valread = recvfrom(sockfd, &dataFrame, framesize, 0, (struct sockaddr *)&client_addr, &addrlen);
        if (valread > 0)
        {
            dataFrame.buffer[BUFFERSIZE - 1] = '\0';

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
            else if (dataFrame.seqNo > expectedSeqNo)
            {
                printf("Out of order Frame-%d received and discarded\n", dataFrame.seqNo);
                ackFrame.seqNo = expectedSeqNo - 1;
                sendto(sockfd, &ackFrame, framesize, 0, (struct sockaddr *)&client_addr, addrlen);
                printf("ACK resent for the last correctly received Frame-%d\n", ackFrame.seqNo);
            }
            else
            {
                printf("Duplicate Frame-%d received\n", dataFrame.seqNo);
                ackFrame.seqNo = dataFrame.seqNo;
                sendto(sockfd, &ackFrame, framesize, 0, (struct sockaddr *)&client_addr, addrlen);
                printf("ACK resent for Frame-%d\n", dataFrame.seqNo);
            }
        }
        else
        {
            perror("recvfrom");
            close(sockfd);
            exit(1);
        }
    }

    printf("All frames received successfully!\n");
    close(sockfd);
    return 0;
}