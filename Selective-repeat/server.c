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
#define PORT 4000
#define TOTALFRAMES 10
#define LOSSPROB 0.2

typedef struct
{
    int valid;
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
    }

    int expectedSeqNo = 0;
    Frame buffer[TOTALFRAMES];
    for (int i = 0; i < TOTALFRAMES; i++)
    {
        buffer[i].valid = 0;
    }

    while (expectedSeqNo < TOTALFRAMES)
    {
        int valread = recvfrom(sockfd, &dataFrame, framesize, 0, (struct sockaddr *)&client_addr, &addrlen);
        if (valread > 0)
        {
            if (dataFrame.seqNo == expectedSeqNo)
            {
                printf("Frame-%d recieved\n", dataFrame.seqNo);
                dataFrame.valid = 1;
                buffer[expectedSeqNo] = dataFrame;
                ackFrame.seqNo = dataFrame.seqNo;
                if ((rand() % 100 / 100.0) > LOSSPROB)
                {
                    sendto(sockfd, &ackFrame, framesize, 0, (struct sockaddr *)&client_addr, addrlen);
                    printf("ACK sent for Frame-%d\n", ackFrame.seqNo);
                }
                else
                {
                    printf("Simulating ACK loss : ACK for Frame-%d not sent\n", ackFrame.seqNo);
                }
                expectedSeqNo++;
                while (buffer[expectedSeqNo].valid == 1)
                {
                    printf("Frame-%d recieved from buffer\n", buffer[expectedSeqNo].seqNo);
                    expectedSeqNo++;
                }
            }
            else if (dataFrame.seqNo >= 0 && dataFrame.seqNo < TOTALFRAMES)
            {
                if (buffer[dataFrame.seqNo].valid == 0)
                {
                    dataFrame.valid = 1;
                    buffer[dataFrame.seqNo] = dataFrame;
                    printf("Out of order Frame-%d recieved and stored in buffer\n", dataFrame.seqNo);
                }
                else
                {
                    printf("Duplicate Frame-%d recieved, ignoring\n", dataFrame.seqNo);
                }
                ackFrame.seqNo = dataFrame.seqNo;
                sendto(sockfd, &ackFrame, framesize, 0, (struct sockaddr *)&client_addr, addrlen);
                printf("ACK sent for out of order Frame-%d\n", dataFrame.seqNo);
            }
            else
            {
                printf("Corrupted Frame recieved !\n");
                ackFrame.seqNo = expectedSeqNo - 1;
                sendto(sockfd, &ackFrame, framesize, 0, (struct sockaddr *)&client_addr, addrlen);
                printf("Last valid ACK(Frame-%d) resent !\n", ackFrame.seqNo);
            }
        }
        else
        {
            perror("recvfrom");
            break;
        }
    }
    printf("All Frames recieved successfully !\n");
    close(sockfd);
    return 0;
}