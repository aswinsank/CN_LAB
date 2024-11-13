#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int main()
{
    int H, L, S;
    printf("Distance Vector Routing using Bellman Ford Algorithm:\n");
    printf("Enter number of hops: ");
    scanf("%d", &H);
    printf("Enter number of links: ");
    scanf("%d", &L);
    printf("Enter the source node (0 to %d): ", H - 1);
    scanf("%d", &S);

    int hops[L], dest[L], wt[L];
    for (int i = 0; i < L; i++)
    {
        printf("\nLink %d: enter source, destination and weight: \n", i + 1);
        scanf("%d %d %d", &hops[i], &dest[i], &wt[i]);
    }

    int dist[H];
    for (int i = 0; i < H; i++)
        dist[i] = INT_MAX;
    dist[S] = 0;
    for (int i = 1; i < H; i++)
    {
        for (int j = 0; j < L; j++)
        {
            int u = hops[j];
            int v = dest[j];
            int weight = wt[j];
            if (dist[u] != INT_MAX && dist[u] + weight < dist[v])
            {
                dist[v] = dist[u] + weight;
            }
        }
    }
    for (int i = 0; i < L; i++)
    {
        int u = hops[i];
        int v = dest[i];
        int weight = wt[i];
        if (dist[u] != INT_MAX && dist[u] + weight < dist[v])
        {
            printf("Network contains negative weight cycle\n");
            return 0;
        }
    }

    printf("\nHop\tDistance from source\n");
    for (int i = 0; i < H; i++)
    {
        printf("%d \t %d\n", i, dist[i]);
    }

    return 0;
}
