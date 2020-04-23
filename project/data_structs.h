

typedef struct pqNode{
    // for priority queue used 
    double priority;
    node_t* node; 
    struct pqNode *next;
  
} pqNode;



pqNode* newPQ_node(double, node_t*); 

void pq_push(pqNode**, pqNode *);

bool isEmpty(pqNode** ); 

pqNode* pop(pqNode** ); 


