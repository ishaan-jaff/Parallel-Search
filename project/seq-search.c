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


bool sequential_dfs(graph_t* graph, int rows, int cols){
  //nature of dfs tells us the max number of traversals will be col+1
  //traverse graph vertically at most col times+1 for the first horizontal traversal
  int max_iter = cols+1;
  int curr_col = cols-1;
  node_t ***nodes = graph->nodes;
  for(int count = 0; count <= max_iter; count++){
    if(count == 0){
      for(int c=0; c<cols; c++){
        if(nodes[0][c]->type == 'E')
          return true;
        if(nodes[0][c]->type != 'S')
          graph->nodes[0][c]->type = 'V';
      }
    }
    else if(count%2==1){ //traverse down
      for(int r=1;r<rows;r++){
        if(nodes[r][curr_col]->type == 'E'){
          return true;
        }
        if(nodes[r][curr_col]->type != 'E')
          nodes[r][curr_col]->type = 'V';
      }
      curr_col--;
    }
    else{ //traverse up
      for(int r=rows-1; r>0; r--){
        if(nodes[r][curr_col]->type == 'E'){
          return true;
        }
        if(nodes[r][curr_col]->type != 'E')
          nodes[r][curr_col]->type = 'V';
      }
      curr_col--;
    }
  }
  graph->nodes = nodes;
  return false;
}
    
 
