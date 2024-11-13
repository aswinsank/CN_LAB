#include <stdio.h>
#include <limits.h>

int main()
{
    int H;
    printf("LINK STATE ROUTING PROTOCOL:\nEnter the number of hops: ");
    scanf("%d", &H);

    int adj[50][50];
    printf("Enter the adjacency matrix:\n");
    for (int i = 0; i < H; i++)
    {
        for (int j = 0; j < H; j++)
        {
            scanf("%d", &adj[i][j]);
        }
    }

    int dist[H], visited[H];
    for (int i = 0; i < H; i++)
    {
        dist[i] = INT_MAX;
        visited[i] = 0;
    }
    dist[0] = 0;

    for (int round = 0; round < H - 1; round++)
    {
        int min = INT_MAX, u = -1;

        for (int v = 0; v < H; v++)
        {
            if (!visited[v] && dist[v] < min)
            {
                min = dist[v];
                u = v;
            }
        }

        if (u == -1)
            break;
        visited[u] = 1;

        for (int v = 0; v < H; v++)
        {
            if (!visited[v] && adj[u][v] && dist[u] != INT_MAX)
            {
                int new_dist = dist[u] + adj[u][v];
                if (new_dist < dist[v])
                {
                    dist[v] = new_dist;
                }
            }
        }
    }

    printf("Vertex\tDistance from Source\n");
    for (int i = 0; i < H; i++)
    {
        printf("%d\t\t%d\n", i, dist[i]);
    }

    return 0;
}
