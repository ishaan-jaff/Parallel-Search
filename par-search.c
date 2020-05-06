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

bool a_star_limited(graph_t* graph, int max_size, pqNode** head); 

bool depth_first_search(graph_t*,int ,int );

void visualize_graph(graph_t* );

double currentSeconds();

void visualize_graph(graph_t* graph);


//******************************* THIS FILE IS SPLIT INTO 3 SECTIONS: BFS, DFS and A*********************************//

//******************************** START OF A* SECTION***************************************************************//

// Call this function to debug A*
// this visualises a priority queue 
void seePQ(pqNode* s, int tid){
  double total_priority = 0.0;
  int num_elems = 0; 
  printf("SEE PQUEUE\n");
  if(s==NULL)
    printf("Head is null\n");
  while(s!=NULL){
    //printf("pos=%d, %d, f=%f\n", s->node->x, s->node->y, s->priority);
    total_priority+= s->priority; 
    num_elems++; 
    s = s->next;
    
  }
  printf("total priority=%f, tid=%d\n", total_priority, tid); 
  printf("Num Elems=%d, tid=%d\n", num_elems, tid); 
}


// Version 3, this was the best and fastest parallel A*, 136x speedup on large graphs  
//This is Step 1 of version 3 
bool a_star_limited(graph_t* graph, int max_size, pqNode** startPQ){
    node_t* end = graph->end;
    int size = 1; 
    while((*startPQ) != NULL && size<max_size){
        node_t* current = pop(&(*startPQ))->node;
        size--; 
        if(current->type=='B')
            continue; 
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
            neighbor_node->time = current->time+1; 
            pqNode* temp = newPQ_node(neighbor_node->f, neighbor_node);
            pq_push(&(*startPQ), temp);
            size++; 
        }
    }
    //seePQ(*startPQ);  use this for debugging 
    return false;
}


// Main component of version 3 
bool parallel_a_star(graph_t* graph){
  // Step 1: Fill PQ with some sequential exploration 
  // Step 2: Distribute nodes in PQ amongst num_threads PQs
  // Step 3: Each thread has its own PQ and executes A* in parallel 
  pqNode* startPQ = newPQ_node(0.0, graph->start);
  int num_threads = 8; 
  int total_nodes = graph->width * graph->height; 
  int limited_size = total_nodes/1000000; 
  // getting multiple of 8 closest to limited size
  limited_size = limited_size + num_threads/2;
  limited_size = limited_size - (limited_size%num_threads);
  if(limited_size<num_threads)
    limited_size = num_threads; 

  printf("LIMITED SIZE=%d\n", limited_size);
  a_star_limited(graph,limited_size, &startPQ); 
  
  // useful for debugging 
  //seePQ(startPQ);

  // Step 2 distrbuting nodes amongst threads
  double priorities[num_threads]; 
  pqNode** threadPQs = malloc(sizeof(pqNode**)*num_threads); 
  for(int i = 0; i<num_threads; i++){
    threadPQs[i] = NULL;
    priorities[i] = 0.0; 
  }

  //PARTITONING OPTIONS
  // Choose one of round robin, greedy or interval partitioning 
  // Round Robin Partitioning had the best performance 
  int curr_thread = 0; 
  for(int i = 0; i<limited_size; i++){
    if(curr_thread>=num_threads)
      curr_thread = 0; 
    pqNode* temp = pop(&(startPQ));
    pq_push(&(threadPQs[curr_thread]), temp); 
    curr_thread++;
  }

  // Greedy Partitioning 
  // uncomment this section if you would like to run greedy partitioning 
  /*
  int curr_thread = 0; 
  int lowest_p = 0; 
  for(int i = 0; i<limited_size; i++){
    if(curr_thread>=num_threads)
      curr_thread = 0; 
    pqNode* temp = pop(&(startPQ));
    priorities[curr_thread]+= temp->priority; 
    pq_push(&(threadPQs[curr_thread]), temp); 
    
    lowest_p = priorities[curr_thread]; 
    for(int k=0; k<num_threads; k++){
      if(priorities[k]<lowest_p){
        lowest_p = priorities[k];
        curr_thread = k;
      }
    }
  }
  */ 


  // This is the Interval Partitioning Scheme that can be used 
  // Uncomment this section if you would like to use interval partitioning 
  // This had a higher std dev
  /*
  int num_per_pq = limited_size/num_threads;
  int rem = (limited_size-(num_per_pq*num_threads));
  for(int i=0; i<num_threads; i++){
    if(i==num_threads-1)
      num_per_pq+=rem;
    int count = 0;
    while(count<num_per_pq){
      pqNode* temp = pop(&(startPQ));
      pq_push(&(threadPQs[i]), temp); 
      count++; 
    }
  }
  */


  // only for debugging purposes 
  // This provides information about the num_threads PQs
  /*
  for(int j=0; j<num_threads; j++){
    seePQ(threadPQs[j], j); 
  }
  */ 
  // End of PARTITIONING OPTIONS 

  // Step 3 starting up threads
  node_t* end = graph->end;
  bool found = 0; 
  unsigned long total = 0; 
  #pragma omp parallel num_threads(num_threads)
  {
    int tid = omp_get_thread_num();
    pqNode* thread_PQ = threadPQs[tid];
    while(thread_PQ != NULL && found==0){
        node_t* current = pop(&thread_PQ)->node;
        #pragma omp atomic
        total++; 

        if(current->type=='B')
            continue; 
        if(current->type=='E'){
            printf("Found at =%d, %d\n", current->x, current->y);
            found = 1; 
            continue; 
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
            neighbor_node->time = current->time+1; 
            neighbor_node->h = ((neighbor_node->x - end->x) * (neighbor_node->x - end->x)) + ((neighbor_node->y - end->y) * (neighbor_node->y - end->y));
            neighbor_node->f = neighbor_node->g + neighbor_node->h;
            pqNode* temp = newPQ_node(neighbor_node->f, neighbor_node);
            pq_push(&thread_PQ, temp);
        }
    }

  }
  //printf("Finished executing\n");
  //printf("Total nodes=%lu\n", total);
  if(found)
    return true; 
  return false; 

}


/*
Version 2 with poor performance and correctness issues 
int* parallel_a_star_step(graph_t* graph, pqNode** startPQ, int* step_info, int num_nodes){
    int new_num_nodes = 0;
    node_t* end = graph->end;
    printf("num of nodes=%d\n", num_nodes);
    pqNode** new_explored = malloc(sizeof(pqNode*)*100000);
    int idx = 0; 
    if(num_nodes>8)
      num_nodes = 8;

    for(int i = 0; i<(num_nodes); i++){
        node_t* current; 
        
        //#pragma omp critical
        current = pop(&(*startPQ))->node;
        
        //#pragma omp atomic        
        if(current->type=='E'){
            printf("Found at =%d, %d\n", current->x, current->y);
            step_info[1] = 1;
            step_info[0] = new_num_nodes;
            //return step_info;
        }
        if(current->type=='B')
          continue; 
        if(current->visited==1)
          continue;
        
        current->visited = 1; 
        current->type = 'V';
        
        //printf("curr node=%d,%d priority=%f\n", current->x, current->y, current->f);
        

        
        int neighbor_positions [8][2] = {{0,1},{1,0},{0,-1},{-1,0},
                                        {1,1}, {1,-1}, {-1,1}, {-1,-1}};
        for(int i=0; i<8; i++){
            int neighbor_x = current->x + neighbor_positions[i][0];
            int neighbor_y = current->y + neighbor_positions[i][1];
            if(neighbor_x < 0 || neighbor_y < 0 || neighbor_x >= graph->height || neighbor_y >= graph->width)
                continue;

            node_t* neighbor_node = graph->nodes[neighbor_x][neighbor_y];
            
            // create the heuristic f,g,h values
            neighbor_node->g = current->g + 1;
            neighbor_node->h = ((neighbor_node->x - end->x) * (neighbor_node->x - end->x)) + ((neighbor_node->y - end->y) * (neighbor_node->y - end->y));
            neighbor_node->f = neighbor_node->g + neighbor_node->h;
            pqNode* temp = newPQ_node(neighbor_node->f, neighbor_node);
            
            //#pragma omp critical
            //pq_push(&nextLevel, temp);
            //new_num_nodes++; 
            new_explored[idx] = temp;
            idx++; 
        }
        
    }
    for(int j = 0; j < idx; j++){
      //pq_push(&startPQ, new_explored[j]);
      pq_push(&(*startPQ), new_explored[j]);
      //printf("j=%d", j);
    }
    
    //seePQ(*startPQ);
    step_info[0] = idx;
    return step_info;
}
bool parallel_a_star(graph_t* graph){
    node_t* start = graph->start;
    pqNode* startPQ = newPQ_node(0.0, start);
    int num_nodes = 1;
    int step_info[3] = {0,0,0};
    bool found = 0;
    while(num_nodes>0 && !found){
        parallel_a_star_step(graph, &startPQ, step_info, num_nodes);
        num_nodes = step_info[0];
        found = step_info[1];
    }
    return false;
}
*/ 
//******************************** END OF A* SECTION***************************************************************//

//******************************** START OF BFS SECTION***************************************************************//
// BREADTH FIRST SEARCH
// Each level is processed in parallel
// The step function processes all the nodes currently in the frontier 
int* parallel_bfs_step(graph_t* graph, node_t** frontier, int head, int tail, int* head_tail, node_t*** all_frontiers, int time){
    int new_tail = tail;
    int found = 0;
    int total = tail-head;
    //printf("Tota nodes=%d\n", total);
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
            current->time = time; 

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
                    //neighbor_node->time = current->time + 1;
                    new_expanded_nodes[index] = neighbor_node;
                    index++;
                    //printf("seen neighbors from tid = %d curr=%d, %d neigh=%d,%d, total=%d, start_idx, end_idx=%d, %d\n", omp_get_thread_num(), current->x, current->y, neighbor_x, neighbor_y, total, start_idx, end_idx);
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


// This is another option to do the BFS parallel step
// This version has a benign race but has better performance when compared to the regular parallel step above 
int* parallel_bfs_fast_step(graph_t* graph, node_t** frontier, int head, int tail, int* head_tail, node_t*** all_frontiers){
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
            if(current->visited==1)
              continue;

            current->visited=1;

            int neighbor_positions [8][2] = {{0,1},{1,0},{0,-1},{-1,0},
                                            {1,1}, {1,-1}, {-1,1}, {-1,-1}};
            for(int i=0; i<8; i++){
                int neighbor_x = current->x + neighbor_positions[i][0];
                int neighbor_y = current->y + neighbor_positions[i][1];
                if(neighbor_x < 0 || neighbor_y < 0 || neighbor_x >= graph->height || neighbor_y >= graph->width)
                    continue;

                node_t* neighbor_node = graph->nodes[neighbor_x][neighbor_y];
                if(neighbor_node->visited==0){
                  new_expanded_nodes[index] = neighbor_node;
                  index++;
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


// The main parallel BFS function called 
// This is called from util.c 
bool parallel_breadth_first_search(graph_t* graph, node_t*** all_frontiers){
    node_t* start = graph->start;
    node_t ** frontier = malloc(sizeof(node_t*)*graph->width*graph->height);
    int head = 0;
    int tail = 0;
    int found;
    frontier[0] = start;
    tail++;
    unsigned long total = 0; 
    int head_tail[3];
    int time = 0; 
    while(head!=tail && found==0){
        //printf("found=%d\n", found);
        parallel_bfs_step(graph, frontier, head, tail, head_tail, all_frontiers, time);
        total = head-tail; 
        time++; 
        //printf("total=%lu\n", total);

        head = head_tail[0];
        tail = head_tail[1];
        found = head_tail[2];
        //printf("Stepped thru\n");

    }
    return false;
}


// Instrumentation code for parallel bfs
// This version has criticial regions which lead to more time spent
/*
double time_visited = 0;
double removing_node_check = 0;
double check_neigh_eq = 0; 
int* parallel_bfs_step(graph_t* graph, node_t** frontier, int head, int tail, int* head_tail, node_t*** all_frontiers, int time){
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
        for(int idx = start_idx; idx <end_idx; idx++){
            double start_remove_node = currentSeconds(); 
            int pos = idx + head;
            if(idx > tail)
                continue;
            node_t* current = frontier[pos];
            current->time = time; 
            if(current->type=='E'){
                printf("Found at=%d,%d\n", current->x, current->y);
                //printf("INSTRUMENTATION\n");
                //printf("marking visited=%f\n", time_visited);
                //printf("removing node=%f\n", removing_node_check);
                //printf("check and eq=%f\n", check_neigh_eq); 
                found=1;
                continue;
            }

            removing_node_check+= (currentSeconds()-start_remove_node); 
            

          
            int neighbor_positions [8][2] = {{0,1},{1,0},{0,-1},{-1,0},
                                            {1,1}, {1,-1}, {-1,1}, {-1,-1}};
            for(int i=0; i<8; i++){
                int neighbor_x = current->x + neighbor_positions[i][0];
                int neighbor_y = current->y + neighbor_positions[i][1];
                if(neighbor_x < 0 || neighbor_y < 0 || neighbor_x >= graph->height || neighbor_y >= graph->width)
                    continue;

                node_t* neighbor_node = graph->nodes[neighbor_x][neighbor_y];

                double start_v = currentSeconds(); 
                if( __sync_bool_compare_and_swap(&(neighbor_node->visited), 0, 1)) {
                    //time_visited+= (currentSeconds()-start_v);
                    //neighbor_node->time = current->time + 1;
                    double neigh_start = currentSeconds(); 
                    #pragma omp critical
                    {
                      frontier[new_tail] = neighbor_node;
                      new_tail++;
                    }
                    check_neigh_eq+= (currentSeconds()- neigh_start); 
                }
                time_visited+= (currentSeconds()-start_v);
            }
        }
    }
    #endif
    head_tail[0] = tail;
    head_tail[1] = new_tail;
    head_tail[2] = found;
    return head_tail;
}
*/ 


//******************************** END OF BFS SECTION***************************************************************//



//******************************** START OF DFS SECTION***************************************************************//

// Final and fastest parallel version of DFS used 
bool parallel_dfs(graph_t* graph, int rows, int cols){
  //nature of dfs tells us the max number of traversals will be col+1
  //traverse graph vertically at most col times+1 for the first horizontal traversal
  int max_iter = cols + 1;
  bool found = false;
  int num_threads = 8;
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
          if(nodes[0][c]->type == 'E'){
            found = true;
            printf("found at=%d, %d\n", 0, c);
          }
          if(nodes[0][c]->type != 'S' && nodes[0][c]->type != 'V')
            graph->nodes[0][c]->type = 'V';
        }
        count++;

        //dir = DOWN;
      }
      else if(count%2 == 1){
        //visualize_graph(graph);

        while(curr_col >= start_col && !found){
          //visualize_graph(graph);
          printf("tid: %d curr_col: %d\n", tid, curr_col);
          //downward traversal of block
          if(dir == DOWN){
            for(int r=1;r<rows;r++){
              if(nodes[r][curr_col]->type == 'E'){
                printf("found at=%d, %d\n", r, curr_col);
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
                printf("found at=%d, %d\n", r, curr_col);
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

// Other versions of DFS tried that did not have great performance
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
/*
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
        if(graph->nodes[0][c]->type != 'S' && graph->nodes[0][c]->type != 'V'){
          graph->nodes[0][c]->type = 'V';
        //  visualize_graph(graph);
        }
      }
    }
    else if(count%2==1){ //traverse down
      for(int r=1;r<rows;r++){
        //if(tid == 1) visualize_graph(graph);
        if(graph->nodes[r][curr_col]->type == 'E'){
          return true;
        }
        if(graph->nodes[r][curr_col]->type != 'E' && graph->nodes[r][curr_col]->type != 'V'){
          graph->nodes[r][curr_col]->type = 'V';
     //     visualize_graph(graph);
        }
      }
      curr_col--;
    }
    else{ //traverse up
      for(int r=rows-1; r>0; r--){
        //if(tid == 1) visualize_graph(graph);
        if(graph->nodes[r][curr_col]->type == 'E'){
          return true;
        }
        if(graph->nodes[r][curr_col]->type != 'E' && graph->nodes[r][curr_col]->type != 'V'){
          graph->nodes[r][curr_col]->type = 'V';
   //       visualize_graph(graph);
        }
      }
      curr_col--;
    }
  }
  return false;
}
bool parallel_dfs(graph_t *graph, int rows, int cols){
  int num_threads = 8;
  char found = 0;
  int cols_per_thread = cols/num_threads;
  int rem = cols%num_threads;
  while(found == 0){
    //printf("found %d\n", found);
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
*/