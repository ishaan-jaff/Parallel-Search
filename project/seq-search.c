#include <stdio.h>
#include <stdlib.h>
#include "seq-search.h"
#include "data_structs.h"
#include "cycletimer.h"
#include "util.h"

pqNode* newPQ_node(double priority, node_t *node);
void pq_push(pqNode **start, pqNode *curr_node);
bool isEmpty(pqNode **start);
pqNode* pop(pqNode **start);

bool a_star(graph_t* graph){
    node_t* start = graph->start;
    node_t* end = graph->end;
    pqNode* startPQ = newPQ_node(0.0, start);

    while(startPQ != NULL){
        node_t* current = pop(&startPQ)->node;
        if(current->type=='E'){
            printf("Found at =%d, %d\n", current->x, current->y);
            return true;
        }

        //printf("curr node=%d,%d\n", current->x, current->y);
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

bool seq_dfs(graph_t* graph, int row, int col){

    //printf("row=%d, col=%d\n", row, col);
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
    if(seq_dfs(graph, row, col+1))
        return true;

    // down
    if(seq_dfs(graph, row+1, col))
        return true;

    // up
    if(seq_dfs(graph, row-1, col))
        return true;

    // left
    if(seq_dfs(graph, row, col-1))
        return true;

    return false;
}


bool seq_breadth_first_search(graph_t* graph){
    node_t* start = graph->start; 
    node_t ** frontier = malloc(sizeof(node_t*)*graph->width*graph->height);
    int head = 0;
    int tail = 0; 
    frontier[0] = start;
    tail++; 

    while(head!=tail){
        for(int idx = head; idx<tail; idx++){
            node_t* current = frontier[idx];
            head++;
            if(current->type=='E'){
                printf("Found at=%d,%d\n", current->x, current->y);
                return true;
            }
            int neighbor_positions [8][2] = {{0,1},{1,0},{0,-1},{-1,0},
                                            {1,1}, {1,-1}, {-1,1}, {-1,-1}};
            for(int i=0; i<8; i++){
                
                int neighbor_x = current->x + neighbor_positions[i][0];
                int neighbor_y = current->y + neighbor_positions[i][1];
                if(neighbor_x < 0 || neighbor_y < 0 || neighbor_x >= graph->height || neighbor_y >= graph->width)
                    continue; 
                
                node_t* neighbor_node = graph->nodes[neighbor_x][neighbor_y];
                if(neighbor_node->type!='V'){
                    if(neighbor_node->type!='E' && neighbor_node->type!='S'){
                        neighbor_node->type = 'V';
                        neighbor_node->time = current->time + 1;
                    }
                    frontier[tail] = neighbor_node;
                    tail++; 
                }
            }
        }

    }
    return false;
}

int* sequential_bfs_step(graph_t* graph, node_t** frontier, int head, int tail, int* head_tail){
  int found = 0;
  int old_head  =head;
  int old_tail = tail;
  for(int idx = old_head; idx<old_tail; idx++){
    //printf("idx=%d, head=%d, tail=%d\n", idx, head, tail);
    node_t* current = frontier[idx];
    head++;
    if(current->type=='E'){
      printf("Found at=%d,%d\n", current->x, current->y);
      found = 1;
      head_tail[0] = head;
      head_tail[1] = tail;
      head_tail[2] = found;
      return head_tail;

    }
    int neighbor_positions [8][2] = {{0,1},{1,0},{0,-1},{-1,0},
                                    {1,1}, {1,-1}, {-1,1}, {-1,-1}};
    for(int i=0; i<8; i++){

      int neighbor_x = current->x + neighbor_positions[i][0];
      int neighbor_y = current->y + neighbor_positions[i][1];
      if(neighbor_x < 0 || neighbor_y < 0 || neighbor_x >= graph->height || neighbor_y >= graph->width)
          continue;

      node_t* neighbor_node = graph->nodes[neighbor_x][neighbor_y];
      if(neighbor_node->type!='V'){
        if(neighbor_node->type!='E' && neighbor_node->type!='S'){
          neighbor_node->type = 'V';
          neighbor_node->time = current->time + 1;
          //printf("in neighbors\n");
        }
        frontier[tail] = neighbor_node;
        tail++;
      }
    }
  }
  head_tail[0] = head;
  head_tail[1] = tail;
  head_tail[2] = found;
  return head_tail;
}
