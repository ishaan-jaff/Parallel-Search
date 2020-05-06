#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "data_structs.h"
#include "util.h"

bool parallel_a_star(graph_t* graph);
bool a_star_limited(graph_t* graph, int max_size, pqNode** start); 
int* parallel_bfs_step(graph_t* graph, node_t** frontier, int head, int tail, int* head_tail, node_t*** all_frontiers);
bool parallel_breadth_first_search(graph_t* graph, node_t*** all_frontiers);
bool parallel_dfs(graph_t* graph, int rows, int cols);
