#include <stdio.h>

int main()
{
    int n, bucket_size, output_rate;
    printf("Enter the number of packets: ");
    scanf("%d", &n);

    int arrival_times[n], packet_sizes[n];
    printf("Enter the packets in the order of their arrival time:\n");
    for (int i = 0; i < n; i++)
    {
        printf("Enter the time and size of packet %d: ", i + 1);
        scanf("%d %d", &arrival_times[i], &packet_sizes[i]);
    }

    printf("Enter the bucket size: ");
    scanf("%d", &bucket_size);
    printf("Enter the output rate: ");
    scanf("%d", &output_rate);

    int current_bucket = 0, current_time = 0, packet_index = 0;
    int max_time = arrival_times[n - 1];

    while (current_time <= max_time || current_bucket > 0)
    {
        printf("\nAt time %d:", current_time);

        while (packet_index < n && arrival_times[packet_index] == current_time)
        {
            int packet_size = packet_sizes[packet_index];
            if (current_bucket + packet_size <= bucket_size)
            {
                current_bucket += packet_size;
                printf("\n%d byte packet is inserted", packet_size);
            }
            else
            {
                printf("\n%d byte packet is discarded", packet_size);
            }
            packet_index++;
        }
        if (current_bucket > 0)
        {
            int transmitted = (current_bucket >= output_rate) ? output_rate : current_bucket;
            current_bucket -= transmitted;
            printf("\n%d bytes transferred", transmitted);
        }
        else
        {
            printf("\nNo packets to transmit");
        }

        printf("\nPackets in the bucket: %d byte(s)\n", current_bucket);
        current_time++;
    }

    return 0;
}
