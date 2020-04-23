#include <stdio.h>
#include <stdlib.h>
#include "data_structs.h"
#include "util.h"

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

graph_t* init_search_space(int width, int height, int *start, int *end){
    printf("Start Init\n");
    graph_t *graph = malloc(sizeof(graph_t));
    graph->nodes = malloc(height * sizeof(node_t***));
    for(int i =0; i<height; i++){
        graph->nodes[i] = malloc(width * sizeof(node_t**));
    }
    // Going through all the nodes 
    for(int i = 0; i<height; i++){
        for(int j = 0; j<width; j++){
            graph->nodes[i][j] = malloc(sizeof(node_t));
            graph->nodes[i][j]->x = i;
            graph->nodes[i][j]->y = j;
            graph->nodes[i][j]->time = 0;
            graph->nodes[i][j]->visited = 0;
            if(i==start[0] && j==start[1]){
                graph->nodes[i][j]->type = 'S';
                graph->nodes[i][j]->f = 0;
                graph->nodes[i][j]->g = 0;
                graph->nodes[i][j]->h = 0;
                graph->start = graph->nodes[i][j];

            }
            else if(i==end[0] && j==end[1]){
                graph->nodes[i][j]->type = 'E';
                graph->nodes[i][j]->f = 0;
                graph->nodes[i][j]->g = 0;
                graph->nodes[i][j]->h = 0;
                graph->end = graph->nodes[i][j];
            }
            else{
                graph->nodes[i][j]->type = 'O';
            }
        }
    }
    graph->width = width;
    graph->height = height;
    graph->start_node = start;
    graph->end_node = end;
  
    printf("%d %d\n", graph->height, graph->width);
    //visualize_graph(graph);
    return graph; 
}

