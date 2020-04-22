#include <stdio.h>
#include <stdlib.h>
#include "run.h"
#include "data_structs.h"
#include "cycletimer.h"


bool a_star(graph_t* graph){
    double startTime = currentSeconds();
    node_t* start = graph->start; 
    node_t* end = graph->end;
    pqNode* startPQ = newPQ_node(0.0, start);
    
    while(startPQ != NULL){
        node_t* current = pop(&startPQ)->node;
        if(current->type=='E'){
            double endTime = currentSeconds();
            printf("Time take = %f", endTime - startTime);
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
    double endTime = currentSeconds();
    printf("Time take = %f", endTime - startTime);
    return false;
}


bool depth_first_search(graph_t* graph, int row, int col){
    
    printf("row=%d, col=%d\n", row, col);
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