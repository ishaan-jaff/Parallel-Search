#ifndef UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include "data_structs.h"

//function to visualize nodes of graph
void visualize_graph(graph_t* graph);

//initializes a graph to search
graph_t* init_search_space(int width, int height, int *start, int *end);

#define UTIL_H
#endif
