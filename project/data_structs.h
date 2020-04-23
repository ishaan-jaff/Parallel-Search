#ifndef DATA_STRUCTS_H
#include "util.h"

typedef struct pqNode{
    // for priority queue used
    double priority;
    node_t* node;
    struct pqNode *next;

} pqNode;

pqNode* newPQ_node(double priority, node_t *node);

void pq_push(pqNode **start, pqNode *curr_node);

bool isEmpty(pqNode **start);

pqNode* pop(pqNode **start);

#define DATA_STRUCTS_H
#endif
