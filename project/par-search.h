#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "data_structs.h"
#include "util.h"

bool parallel_a_star(graph_t* graph);
int* parallel_bfs_step(graph_t* graph, node_t** frontier, int head, int tail, int* head_tail);
bool parallel_breadth_first_search(graph_t* graph);
bool parallel_dfs(graph_t* graph, int rows, int cols);
