#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "data_structs.h"
#include "seq-search.h"
#include "util.h"

pqNode* newPQ_node(double priority, node_t *node);
void pq_push(pqNode **start, pqNode *curr_node);
bool isEmpty(pqNode **start);
pqNode* pop(pqNode **start);

bool a_star(graph_t*);

bool depth_first_search(graph_t*,int ,int );

void visualize_graph(graph_t* );

double currentSeconds();

void visualize_graph(graph_t* graph);


int* parallel_a_star_step(graph_t* graph, pqNode* startPQ, int* step_info, int num_nodes){
    int new_num_nodes = num_nodes;
    node_t* end = graph->end;
    printf("num of nodes=%d\n", num_nodes);
    for(int i = 0; i<num_nodes; i++){
        node_t* current = pop(&startPQ)->node;
        new_num_nodes--;
        if(current->type=='E'){
            printf("Found at =%d, %d\n", current->x, current->y);
            step_info[1] = 1;
            step_info[0] = new_num_nodes;
            return step_info;
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
            new_num_nodes++;
        }
    }
    step_info[0] = new_num_nodes;
    return step_info;
}
bool parallel_a_star(graph_t* graph){
    node_t* start = graph->start;
    pqNode* startPQ = newPQ_node(0.0, start);
    int num_nodes = 1;
    int step_info[3] = {0,0,0};
    bool found = 0;
    while(startPQ != NULL && !found){
        parallel_a_star_step(graph, startPQ, step_info, num_nodes);
        num_nodes = step_info[0];
        found = step_info[1];
    }
    return false;
}




int* parallel_bfs_step(graph_t* graph, node_t** frontier, int head, int tail, int* head_tail, node_t*** all_frontiers){
    int new_tail = tail;
    int found = 0;
    int total = tail-head;
    // Expand nodes on the frontier in parallel
    // each thread given some amount of nodes from the frontier
    // after it adds nodes to its own frontier, it is copied to the shared frontier
    #if OMP
    #pragma omp parallel num_threads(8)
    {
        int tid = omp_get_thread_num();
        int num_threads = omp_get_num_threads();
        int index = 0;

        node_t** new_expanded_nodes = all_frontiers[tid];

        int nodes_per_thrd = total/num_threads;
        int remainder = total - (num_threads*nodes_per_thrd);
        int start_idx = nodes_per_thrd * tid;
        int end_idx = start_idx + nodes_per_thrd;
        if(tid==num_threads-1)
            end_idx += remainder;

        //printf("total=%d\n", total);
        for(int idx = start_idx; idx <end_idx; idx++){
            int pos = idx + head;
            if(idx > tail)
                continue;
            node_t* current = frontier[pos];

            //head++;
            if(current->type=='E'){
                printf("Found at=%d,%d\n", current->x, current->y);
                found=1;
                continue;
            }

            int neighbor_positions [8][2] = {{0,1},{1,0},{0,-1},{-1,0},
                                            {1,1}, {1,-1}, {-1,1}, {-1,-1}};
            for(int i=0; i<8; i++){
                int neighbor_x = current->x + neighbor_positions[i][0];
                int neighbor_y = current->y + neighbor_positions[i][1];
                if(neighbor_x < 0 || neighbor_y < 0 || neighbor_x >= graph->height || neighbor_y >= graph->width)
                    continue;

                node_t* neighbor_node = graph->nodes[neighbor_x][neighbor_y];

                if( __sync_bool_compare_and_swap(&(neighbor_node->visited), 0, 1)) {
                    neighbor_node->time = current->time + 1;
                    new_expanded_nodes[index] = neighbor_node;
                    index++;
                    //printf("seen neighbors from tid = %d curr=%d, %d neigh=%d,%d, total=%d, start_idx, end_idx=%d, %d\n", omp_get_thread_num(), current->x, current->y, neighbor_x, neighbor_y, total, start_idx, end_idx);
                }

                #pragma omp critical
                {
                    neighbor_node->visited = 1;

                }

            }
        }
        #pragma omp critical
       {
            memcpy(&frontier[new_tail], new_expanded_nodes, (index+1)*sizeof(node_t*));
            new_tail += index;
        }

    }
    #endif
    //printf("new head=%d, new tail=%d\n", tail, new_tail);
    head_tail[0] = tail;
    head_tail[1] = new_tail;
    head_tail[2] = found;
    return head_tail;

}


bool parallel_breadth_first_search(graph_t* graph, node_t*** all_frontiers){
    node_t* start = graph->start;
    node_t ** frontier = malloc(sizeof(node_t*)*graph->width*graph->height);
    int head = 0;
    int tail = 0;
    int found;
    frontier[0] = start;
    tail++;
    int head_tail[3];
    while(head!=tail && found==0){
        //printf("found=%d\n", found);
        parallel_bfs_step(graph, frontier, head, tail, head_tail, all_frontiers);


        head = head_tail[0];
        tail = head_tail[1];
        found = head_tail[2];
        //printf("Stepped thru\n");

    }
    return false;
}

/*
bool parallel_dfs(graph_t* graph, int rows, int cols){
  //nature of dfs tells us the max number of traversals will be col+1
  //traverse graph vertically at most col times+1 for the first horizontal traversal
  bool found = false;
  int max_iter = cols+1;
  int curr_col = cols-1;
  omp_set_num_threads(8);
  for(int count = 0; count <= max_iter; count++){
    if(count == 0){
      #if OMP
      #pragma omp parallel for
      #endif
      for(int c=0; c<cols; c++){
        if(graph->nodes[0][c]->type == 'E')
          found = true;
        if(graph->nodes[0][c]->type != 'S' && graph->nodes[0][c]->type != 'V')
          graph->nodes[0][c]->type = 'V';
      }
    }
    else if(count%2==1){ //traverse down
      #if OMP
      #pragma omp parallel for
      #endif
      for(int r=1;r<rows;r++){
      //  visualize_graph(graph);
        if(graph->nodes[r][curr_col]->type == 'E'){
          found = true;
        }
        if(graph->nodes[r][curr_col]->type != 'E' && graph->nodes[r][curr_col]->type != 'V')
          graph->nodes[r][curr_col]->type = 'V';
      }
      curr_col--;
    }
    else{ //traverse up
      #if OMP
      #pragma omp parallel for
      #endif
      for(int r=rows-1; r>0; r--){
     //   visualize_graph(graph);
        if(graph->nodes[r][curr_col]->type == 'E'){
          found = true;
        }
        if(graph->nodes[r][curr_col]->type != 'E' && graph->nodes[r][curr_col]->type != 'V')
          graph->nodes[r][curr_col]->type = 'V';
      }
      curr_col--;
    }
    if(found) return true;
  }
  return false;
}

*/

bool dfs_helper(graph_t* graph, int rows, int cols, int start_col, int tid){
  //nature of dfs tells us the max number of traversals will be col+1
  //traverse graph vertically at most col times+1 for the first horizontal traversal
  int max_iter = cols+1;
  int curr_col = cols-1;
  for(int count = 0; count <= max_iter; count++){
    if(count == 0){
      for(int c=start_col; c<cols; c++){
        //if(tid == 1) visualize_graph(graph);
        if(graph->nodes[0][c]->type == 'E')
          return true;
        if(graph->nodes[0][c]->type != 'S' && graph->nodes[0][c]->type != 'V')
          graph->nodes[0][c]->type = 'V';
      }
    }
    else if(count%2==1){ //traverse down
      for(int r=1;r<rows;r++){
        //if(tid == 1) visualize_graph(graph);
        if(graph->nodes[r][curr_col]->type == 'E'){
          return true;
        }
        if(graph->nodes[r][curr_col]->type != 'E' && graph->nodes[r][curr_col]->type != 'V')
          graph->nodes[r][curr_col]->type = 'V';
      }
      curr_col--;
    }
    else{ //traverse up
      for(int r=rows-1; r>0; r--){
        //if(tid == 1) visualize_graph(graph);
        if(graph->nodes[r][curr_col]->type == 'E'){
          return true;
        }
        if(graph->nodes[r][curr_col]->type != 'E' && graph->nodes[r][curr_col]->type != 'V')
          graph->nodes[r][curr_col]->type = 'V';
      }
      curr_col--;
    }
  }
  return false;
}

bool parallel_dfs(graph_t *graph, int rows, int cols){
  int num_threads = 2;
  char found = 0;
  int cols_per_thread = cols/num_threads;
  int rem = cols%num_threads;
  while(found == 0){
    printf("found %d\n", found);
    #if OMP
    #pragma omp parallel num_threads(num_threads)
    {
      int nthreads = omp_get_num_threads();
      int tid = omp_get_thread_num();
      int start_col = tid*cols_per_thread;
      int end_col = start_col + cols_per_thread;
      if(tid == nthreads-1) end_col += rem;
      bool res = dfs_helper(graph, rows, end_col, start_col, tid);
      #pragma omp critical
      {if(res) found |= (res << tid);}
    }
    #endif
  }
  if(found > 0) return true;
  return false;
}


/*
bool parallel_dfs(graph_t* graph, int rows, int cols){
  //nature of dfs tells us the max number of traversals will be col+1
  //traverse graph vertically at most col times+1 for the first horizontal traversal
  int max_iter = cols + 1;
  bool found = false;
  int num_threads = 3;
  node_t ***nodes = graph->nodes;

//  omp_set_dynamic(0);
  omp_set_num_threads(num_threads);
  //num_threads = omp_get_num_threads();
  printf("num_threads %d\n", num_threads);
  int rem_cols = cols%num_threads;
 // printf("rem_cols: %d num_threads %d\n", rem_cols,num_threads);
  int cols_per_thread = cols/num_threads;
  printf("per thread %d\n", cols_per_thread);
  int count = 0;
  #if OMP
  while(!found){
  #pragma omp parallel num_threads(8)
    {
      int tid = omp_get_thread_num();
      //printf("tid: %d, count %d\n", tid, count);
      int start_col = tid*cols_per_thread;
      int end_col = (tid+1)*cols_per_thread;
      int curr_col = end_col-1;
      dir_t dir = HORIZ;
      if(tid == num_threads-1) end_col +=rem_cols;
      //horizontal traversal of block
      if(count == 0){
        for(int c=start_col; c<end_col; c++){
          //          printf("nodes[0][%d] tid: %d \n", c, tid);
          if(nodes[0][c]->type == 'E')
            found = true;
          if(nodes[0][c]->type != 'S' && nodes[0][c]->type != 'V')
            graph->nodes[0][c]->type = 'V';
        }
        count++;

        //dir = DOWN;
      }
      else if(count%2 == 1){
        visualize_graph(graph);

        while(curr_col >= start_col && !found){
          visualize_graph(graph);
          printf("tid: %d curr_col: %d\n", tid, curr_col);
          //downward traversal of block
          if(dir == DOWN){
            for(int r=1;r<rows;r++){
              if(nodes[r][curr_col]->type == 'E'){
                printf("found going donw\n");
                found = true;
              }
              if(nodes[r][curr_col]->type != 'E' && nodes[r][curr_col]->type != 'V')
                nodes[r][curr_col]->type = 'V';
            }
            curr_col--;
            count++;
            //dir = UP;
      }
          //upward traversal of block
      else{
            for(int r=rows-1; r>0; r--){
              if(nodes[r][curr_col]->type == 'E'){
                printf("found going up\n");
                found = true;
              }
              if(nodes[r][curr_col]->type != 'E'&& nodes[r][curr_col]->type != 'V')
                nodes[r][curr_col]->type = 'V';
            }
            curr_col--;
            dir = DOWN;
            count++;
          }
      }
    }
  }
  #endif
  graph->nodes = nodes;
  if(found) return true;
}
  return false;
}
*/
