
#include <stdio.h>
#include <stdlib.h>
#include "data_structs.h"
#include "util.h"

bool a_star(graph_t* graph);
int* sequential_bfs_step(graph_t* graph, node_t** frontier, int head, int tail, int* head_tail);
bool seq_dfs(graph_t* graph, int row, int col);
