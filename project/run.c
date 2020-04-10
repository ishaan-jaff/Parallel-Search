#include <stdio.h>
#include <stdlib.h>
#include "run.h"


bool depth_first_search(graph_t* graph, int row, int col){
    

    if(row<0 || col <0 || row >= graph->height || col >= graph->width)
        return false; 
    
    // visited this node before, do not need to further search 
    if(graph->nodes[row][col].type=="V")
        return false;
    
    if(graph->nodes[row][col].type=="E")
        return true;
    
    // if not visited mark as visited 
    graph->nodes[row][col].type = "V";

    // right 
    if(depth_first_search(graph, row, col+1))
        return true;
    
    // down
    if(depth_first_search(graph, row+1, col))
        return true;
    
    // left 
    if(depth_first_search(graph, row, col-1))
        return true;
    
    // up 
    if(depth_first_search(graph, row-1, col))
        return true;
    
    return false; 
}


void visualize_graph(graph_t* graph){
    // Going through all the nodes 
    printf("VISUALIZING\n");
    printf("S is Start\n");
    printf("E is End\n");
    printf("V is Visited\n");
    printf("0 is a regular node\n\n\n");
    for(int i = 0; i<graph->height; i++){
        for(int j = 0; j<graph->width; j++){
            if(j==graph->width-1){
                printf("%s\n", graph->nodes[i][j].type);
            }
            else{
                printf("%s", graph->nodes[i][j].type);
            }

        }
    }
    printf("\n\n\n");
    return;

}

graph_t* init_search_space(int width, int height, int *start, int *end){
    printf("Start Init\n");
    graph_t *graph = malloc(sizeof(graph_t*));
    graph->width = width;
    graph->height = height;
    graph->start_node = start;
    graph->end_node = end;
    graph->nodes = malloc(height * sizeof(node_t*));
    for(int i =0; i<height; i++){
        graph->nodes[i] = malloc(width * sizeof(node_t));
    }

    // Going through all the nodes 
    for(int i = 0; i<height; i++){
        for(int j = 0; j<width; j++){
            if(i==start[0] && j==start[1]){
                graph->nodes[i][j].type = "S";
            }
            else if(i==end[0] && j==end[1]){
                graph->nodes[i][j].type = "E";
            }
            else{
                graph->nodes[i][j].type = "O";
            }
        }
    }
    visualize_graph(graph);
    return graph; 
}


void start_search(int width, int height, int *start, int *end){
    printf("Start Search\n");
    graph_t *graph = init_search_space(width, height, start, end);
    printf("Depth First Search\n");
    depth_first_search(graph, graph->start_node[0], graph->start_node[1]);
    visualize_graph(graph);
}



int main() {
    printf("hi world\n");
    int width = 40;
    int height = 40;
    int start[2] = {0,0};
    int end[2] = {30,34};
    start_search(width, height, start, end);
}


