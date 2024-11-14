#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/socket.h>

#define PORT 4000
#define IP "127.0.0.1"
#define WINDOWSIZE 4
#define BUFFERSIZE 1024
#define TOTALFRAMES 10
#define TIMEOUT 2

typedef struct
{
    int valid;
    int seqNo;
    char buffer[BUFFERSIZE];
} Frame;

int main()
{
    int sockfd;
    struct sockaddr_in server_addr;
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
    server_addr.sin_addr.s_addr = inet_addr(IP);

    struct timeval tv;
    tv.tv_sec = TIMEOUT;
    tv.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
    {
        perror("setsockopt");
        exit(1);
    }

    int base = 0;
    int nextSeqNo = 0;
    Frame buffer[TOTALFRAMES];
    for (int i = 0; i < TOTALFRAMES; i++)
    {
        buffer[i].valid = 0;
    }

    while (base < TOTALFRAMES)
    {
        while (nextSeqNo < base + WINDOWSIZE && nextSeqNo < TOTALFRAMES)
        {
            dataFrame.seqNo = nextSeqNo;
            sendto(sockfd, &dataFrame, framesize, 0, (struct sockaddr *)&server_addr, addrlen);
            printf("Frame-%d sent\n", dataFrame.seqNo);
            buffer[nextSeqNo] = dataFrame;
            nextSeqNo++;
        }
        int valread = recvfrom(sockfd, &ackFrame, framesize, 0, (struct sockaddr *)&server_addr, &addrlen);
        if (valread > 0)
        {
            if (ackFrame.seqNo >= 0 && ackFrame.seqNo < TOTALFRAMES)
            {
                printf("ACK recieved for Frame-%d\n", ackFrame.seqNo);
                ackFrame.valid = 1;
                buffer[ackFrame.seqNo] = ackFrame;
                if (ackFrame.seqNo == base)
                {
                    base++;
                    while (base < TOTALFRAMES && buffer[base].valid == 1)
                    {
                        printf("Sliding window : Frame-%d already acknowledged\n", base);
                        base++;
                    }
                }
            }
            else
            {
                printf("Invalid ACK recieved\n");
            }
        }
        else
        {
            printf("Timeout ! Resending frames from %d to %d\n", base, nextSeqNo - 1);
            for (int i = base; i < nextSeqNo && i < TOTALFRAMES; i++)
            {
                if (buffer[i].valid == 0)
                {
                    sendto(sockfd, &buffer[i], framesize, 0, (struct sockaddr *)&server_addr, addrlen);
                    printf("Resent Frame-%d\n", buffer[i].seqNo);
                }
            }
        }
    }
    printf("All frames sent and acknowledged !\n");
}