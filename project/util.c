#include <stdio.h>
#include <stdlib.h>
#include "data_structs.h"

void visualize_graph(graph_t* graph){
    // Going through all the nodes 
    printf("VISUALIZING\n");
    printf("S is Start\n");
    printf("E is End\n");
    printf("V is Visited\n");
    printf("0 is a regular node\n\n\n");
    
    printf("%d %d\n", graph->height, graph->width);
    for(int i = 0; i<graph->height; i++){
        for(int j = 0; j<graph->width; j++){
            int t = graph->nodes[i][j]->time;
            if(graph->nodes[i][j]==NULL)
                printf("NULL\n");
            if(j==graph->width-1){
                if(t<10){
                    printf("0%d\n", t);
                }
                else{
                    printf("%d\n", t);
                }
                
            }
            else{
                if(t<10){
                    printf("0%d", t);
                }
                else{
                    printf("%d", t);
                }
            }

        }
    }
    printf("\n\n\n");
    return;

}
