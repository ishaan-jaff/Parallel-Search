#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <math.h>


typedef struct {
    char* type;
} node_t;

typedef struct {
    /* General parameters */
    node_t **nodes;
    int width;
    int height; 
    int *start_node; // start search from here 
    int *end_node;  //end search here 

} graph_t;