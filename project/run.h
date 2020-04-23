#ifndef RUN_H
#define RUN_H

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

#endif
