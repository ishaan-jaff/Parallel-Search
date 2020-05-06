#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "data_structs.h"


pqNode* newPQ_node(double priority, node_t *node){
    pqNode* temp = malloc(sizeof(pqNode));
    temp->node = node;
    temp->priority = priority;
    temp->next = NULL;
    return temp;
}
void pq_push(pqNode** start, pqNode *curr_node){
    pqNode *node = *(start);
    if(node==NULL){
        *start = curr_node;
        curr_node->next = NULL; 
    }
    //printf("start p=%f\n", node->priority);
    else if(node->priority > curr_node->priority){
        curr_node->next = node;
        *start = curr_node;
    }

    else {
        while (node->next != NULL && node->next->priority > curr_node->priority) {
            node = node->next;
        }
        // Either at the ends of the list
        // or at required position
        curr_node->next = node->next;
        node->next = curr_node;
    }

}

pqNode* pq_push_return_head(pqNode** start, pqNode *curr_node){
    pqNode *node = *(start);
    if(node==NULL){
        *start = curr_node;
    }
    //printf("start p=%f\n", node->priority);
    else if(node->priority > curr_node->priority){
        curr_node->next = node;
        *start = curr_node;
    }

    else {
        while (node->next != NULL && node->next->priority > curr_node->priority) {
            node = node->next;
        }
        // Either at the ends of the list
        // or at required position
        curr_node->next = node->next;
        node->next = curr_node;
    }
    return *start;

}


bool isEmpty(pqNode** start)
{
    return (*start) == NULL;
}

pqNode* pop(pqNode** start)
{
    pqNode* head = *start;
    (*start) = (*start)->next;
    return head;
}

void test_pq(){
    node_t* d1 = malloc(sizeof(node_t*));
    //d1.type = 'O';
    //d1.x = 0;

    pqNode* startPQ = newPQ_node(10, d1);
    pqNode* t1 = newPQ_node(11, d1);
    pqNode* t2 = newPQ_node(13, d1);
    pqNode* t3 = newPQ_node(2, d1);
    pqNode* t4 = newPQ_node(1, d1);

    pq_push(&startPQ, t1);
    pq_push(&startPQ, t2);
    pq_push(&startPQ, t3);
    pq_push(&startPQ, t4);

    pqNode *n = startPQ;
    while(n!=NULL){
        printf("Curr priority=%f\n", n->priority);
        n = n->next;
    }

    pqNode* removed = pop(&startPQ);
    printf("removed priority=%f\n", removed->priority);
    removed = pop(&startPQ);
    printf("removed priority=%f\n", removed->priority);
    removed = pop(&startPQ);
    printf("removed priority=%f\n", removed->priority);

    n = startPQ;
    while(n!=NULL){
        printf("Curr priority=%f\n", n->priority);
        n = n->next;
    }
    return;
}


/*
int main(){
    test_pq();
    return 0;
}
*/





