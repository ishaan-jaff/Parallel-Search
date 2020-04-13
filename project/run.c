#include <stdio.h>
#include <stdlib.h>
#include "run.h"
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
            
            if(graph->nodes[i][j]==NULL)
                printf("NULL\n");
            if(j==graph->width-1){
                printf("%c\n", graph->nodes[i][j]->type);
            }
            else{
                printf("%c", graph->nodes[i][j]->type);
            }

        }
    }
    printf("\n\n\n");
    return;

}

bool depth_first_search(graph_t* graph, int row, int col){
    
    printf("row=%d, col=%d\n", row, col);
    visualize_graph(graph);
    if(row<0 || col <0 || row >= graph->height || col >= graph->width)
        return false; 
    
    // visited this node before, do not need to further search 
    if(graph->nodes[row][col]->type=='V')
        return false;
    
    if(graph->nodes[row][col]->type=='E')
        return true;
    
    // if not visited mark as visited 
    graph->nodes[row][col]->type = 'V';

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


bool a_star(graph_t* graph){
    node_t* start = graph->start; 
    node_t* end = graph->end;
    pqNode* startPQ = newPQ_node(0.0, start);
    
    while(startPQ != NULL){
        node_t* current = pop(&startPQ)->node;
        if(current->type=='E'){
            return true;
        }
        
        printf("curr node=%d,%d\n", current->x, current->y);
        current->type = 'V';
        int neighbor_positions [8][2] = {{0,1},{1,0},{0,-1},{-1,0},
                                        {1,1}, {1,-1}, {-1,1}, {-1,-1}};
        for(int i=0; i<8; i++){
            int neighbor_x = current->x + neighbor_positions[i][0];
            int neighbor_y = current->y + neighbor_positions[i][1];
            if(neighbor_x < 0 || neighbor_y < 0 || neighbor_x >= graph->height || neighbor_y >= graph->width)
                continue; 
           
            node_t* neighbor_node = graph->nodes[neighbor_x][neighbor_y];
            if(neighbor_node->type=='V')
                continue;
           
            // create the heuristic f,g,h values 
            neighbor_node->g = current->g + 1;
            neighbor_node->h = ((neighbor_node->x - end->x) * (neighbor_node->x - end->x)) + ((neighbor_node->y - end->y) * (neighbor_node->y - end->y));
            neighbor_node->f = neighbor_node->g + neighbor_node->h;
            pqNode* temp = newPQ_node(neighbor_node->f, neighbor_node);
            pq_push(&startPQ, temp);
        }
    }
    return false;
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
    visualize_graph(graph);
    return graph; 
}


void start_search(int width, int height, int *start, int *end){
    printf("Start Search\n");
    graph_t *graph = init_search_space(width, height, start, end);
    //printf("Depth First Search\n");
    /*if(depth_first_search(graph, graph->start_node[0], graph->start_node[1]))
        printf("Found\n");*/
    printf("A* Search\n");
    a_star(graph);
    visualize_graph(graph);
}



int main() {
    printf("hi world\n");
    int width = 100;
    int height = 100;
    int start[2] = {12,60};
    int end[2] = {99,13};
    start_search(width, height, start, end);
}


