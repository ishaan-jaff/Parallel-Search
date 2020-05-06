#ifndef UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <math.h>


typedef struct {
    char type;
    // used for A* search
    // f = g + h;
    double h;
    double g;
    double f;


    // positions of the node
    int x;
    int y;

    int t_id;
    int time;
    bool visited;


} node_t;


typedef struct {
    /* General parameters */
    node_t ***nodes;
    node_t *start;
    node_t *end;
    int width;
    int height;
    int *start_node; // start search from here
    int *end_node;  //end search here

} graph_t;


//function to visualize nodes of graph
void visualize_graph(graph_t* graph);

typedef enum {
  BFS_SEQ,
  BFS_PAR,
  DFS_SEQ,
  DFS_PAR,
  A_SEQ,
  A_PAR,
} search_type_t;

typedef enum {
  UP,
  DOWN,
  HORIZ,
} dir_t;



//initializes a graph to search
graph_t* init_search_space(int width, int height, int *start, int *end);

void start_search(int width, int height, int *start, int *end, search_type_t search_type);
#define UTIL_H
#endif
