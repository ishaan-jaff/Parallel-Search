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

bool parallel_a_star(graph_t* graph){
    double startTime = currentSeconds();
    node_t* start = graph->start;
    node_t* end = graph->end;
    pqNode* startPQ = newPQ_node(0.0, start);

    while(startPQ != NULL){
        node_t* current = pop(&startPQ)->node;
        if(current->type=='E'){
            double endTime =  currentSeconds();
            printf("Found at=%d,%d\n", current->x, current->y);
            printf("Time taken =%f\n", endTime-startTime);
            return true;
        }

        printf("curr node=%d,%d\n", current->x, current->y);
        current->type = 'V';
        current->t_id = 9;
        int neighbor_positions [8][2] = {{0,1},{1,0},{0,-1},{-1,0},
                                        {1,1}, {1,-1}, {-1,1}, {-1,-1}};
        #if OMP
        #pragma omp parallel for
        #endif
        for(int i=0; i<8; i++){

            int neighbor_x = current->x + neighbor_positions[i][0];
            int neighbor_y = current->y + neighbor_positions[i][1];
            if(neighbor_x < 0 || neighbor_y < 0 || neighbor_x >= graph->height || neighbor_y >= graph->width)
                continue;


            node_t* neighbor_node = graph->nodes[neighbor_x][neighbor_y];
            if(neighbor_node->t_id!=9)
#if OMP
                neighbor_node->t_id = omp_get_thread_num();
#endif
            if(neighbor_node->type=='V')
                continue;

            // create the heuristic f,g,h values
            neighbor_node->g = current->g + 1;
            neighbor_node->h = ((neighbor_node->x - end->x) * (neighbor_node->x - end->x)) + ((neighbor_node->y - end->y) * (neighbor_node->y - end->y));
            neighbor_node->f = neighbor_node->g + neighbor_node->h;
            pqNode* temp = newPQ_node(neighbor_node->f, neighbor_node);

            #if OMP
            #pragma omp critical
            #endif
            pq_push(&startPQ, temp);
        }
    }
    double endTime =  currentSeconds();
    printf("Time taken =%f\n", endTime-startTime);

    return false;
}


bool breadth_first_search(graph_t* graph){
    double startTime = currentSeconds();
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
                printf("Time taken=%f\n", currentSeconds()-startTime);
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
    printf("Time taken=%f\n", currentSeconds()-startTime);
    return false;
}



int* parallel_bfs_step(graph_t* graph, node_t** frontier, int head, int tail, int* head_tail){
    int new_tail = tail;
    int found = 0;
    //int total = tail-head;

    // Expand nodes on the frontier in paraallel
    // each thread given some amount of nodes from the frontier
    // after it adds nodes to its own frontier, it is copied to the shared frontier
    #if OMP
    #pragma omp parallel num_threads(8)
    {
        int tid = omp_get_thread_num();
        int num_threads = omp_get_num_threads();
        int index = 0;

        node_t** new_expanded_nodes = malloc(sizeof(node_t*)*((20*total)/num_threads));
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


                }
                //printf("seen neighbors from tid = %d curr=%d, %d neigh=%d,%d, total=%d, start_idx, end_idx=%d, %d\n", omp_get_thread_num(), current->x, current->y, neighbor_x, neighbor_y, total, start_idx, end_idx);
            }
        }
        #pragma omp critical
       {
            memcpy(&frontier[new_tail], new_expanded_nodes, (index+1)*sizeof(node_t*));
            new_tail += index;
        }
        if(new_expanded_nodes)
            free(new_expanded_nodes);
    }
#endif
    //printf("new head=%d, new tail=%d\n", tail, new_tail);
    head_tail[0] = tail;
    head_tail[1] = new_tail;
    head_tail[2] = found;
    return head_tail;

}

/*
int* parallel_bfs_step2(graph_t* graph, node_t*** frontier, int head, int tail, int* head_tail){
    int new_tail = tail;
    int found = 0;

    //printf("head=%d, tail=%d\n", head, tail);

    // Expand nodes on the frontier in paraallel
    //each thread given its own frontier
    // after it adds nodes to its own frontier, it is copied to the shared frontier
    #pragma omp parallel num_threads(8)
    {
        int tid = omp_get_thread_num();
        node_t** frontier_node = frontier[tid];
        int num_threads = omp_get_num_threads();
        int index = 0;
        int total = tail-head;
        node_t** new_expanded_nodes = malloc(sizeof(node_t*)*((20*total)/num_threads));
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


                }
                //printf("seen neighbors from tid = %d curr=%d, %d neigh=%d,%d, total=%d, start_idx, end_idx=%d, %d\n", omp_get_thread_num(), current->x, current->y, neighbor_x, neighbor_y, total, start_idx, end_idx);
            }
        }

        #pragma omp critical
        {
            memcpy(&frontier[new_tail], new_expanded_nodes, (index+1)*sizeof(node_t*));
            new_tail += index;
        }
        if(new_expanded_nodes)
            free(new_expanded_nodes);
    }
    //printf("new head=%d, new tail=%d\n", tail, new_tail);
    head_tail[0] = tail;
    head_tail[1] = new_tail;
    head_tail[2] = found;
    return head_tail;

}
*/

bool parallel_breadth_first_search(graph_t* graph){
    double startTime = currentSeconds();
    node_t* start = graph->start;
    node_t ** frontier = malloc(sizeof(node_t*)*graph->width*graph->height);
    int head = 0;
    int tail = 0;
    int found;
    frontier[0] = start;
    tail++;
    int head_tail[3];
    int total;

    while(head!=tail && found==0){
        //printf("found=%d\n", found);
        total = tail-head;
        printf("total=%d\n", total);
        sequential_bfs_step(graph, frontier, head, tail, head_tail);

        head = head_tail[0];
        tail = head_tail[1];
        found = head_tail[2];
        //printf("Stepped thru\n");

    }
    printf("Time taken=%f\n", currentSeconds()-startTime);
    return false;
}

bool parallel_dfs(graph_t* graph, int rows, int cols){
  //nature of dfs tells us the max number of traversals will be col+1
  //traverse graph vertically at most col times+1 for the first horizontal traversal
  bool found = false;
  int max_iter = cols+1;
  int curr_col = cols-1;
  node_t ***nodes = graph->nodes;
  #if OMP
  #pragma omp parallel num_threads(6)
  #endif
  for(int count = 0; count <= max_iter; count++){
    if(count == 0){
      #if OMP
      #pragma omp parallel for
      #endif
      for(int c=0; c<cols; c++){
        if(nodes[0][c]->type == 'E')
          found = true;
        if(nodes[0][c]->type != 'S')
          graph->nodes[0][c]->type = 'V';
      }
    }
    else if(count%2==1){ //traverse down
      #if OMP
      #pragma omp parallel for
      #endif
      for(int r=1;r<rows;r++){
        if(nodes[r][curr_col]->type == 'E'){
          found = true;
        }
        if(nodes[r][curr_col]->type != 'E')
          nodes[r][curr_col]->type = 'V';
      }
      curr_col--;
    }
    else{ //traverse up
      #if OMP
      #pragma omp parallel for
      #endif
      for(int r=rows-1; r>0; r--){
        if(nodes[r][curr_col]->type == 'E'){
          found = true;
        }
        if(nodes[r][curr_col]->type != 'E')
          nodes[r][curr_col]->type = 'V';
      }
      curr_col--;
    }
    if(found) return true;
  }
  graph->nodes = nodes;
  return false;
}
/*
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
*

void start_search(int width, int height, int *start, int *end){
    printf("Start Search\n");
    graph_t *graph = init_search_space(width, height, start, end);
    //printf("Depth First Search\n");
    //if(depth_first_search(graph, graph->start_node[0], graph->start_node[1]))
    //printf("Found\n");
    //printf("A* Search\n");
    //a_star(graph);
    //printf("parallel A*\n");
    //parallel_a_star(graph);

    //breadth_first_search(graph);
    parallel_breadth_first_search(graph);

    //visualize_graph(graph);

}



int main() {
    printf("hi world\n");

    int width = 10000;
    int height = 1000;
    int start[2] = {0,0};
    int end[2] = {9999, 999};
    start_search(width, height, start, end);
    return 0;
}

*/

