#include <stdio.h>
#include <stdlib.h>
#include "data_structs.h"
#include "util.h"
#include "cycletimer.h"
#include "seq-search.h"
#include "par-search.h"

//sequential algorithm function prototypes
bool a_star(graph_t* graph);
int* sequential_bfs_step(graph_t* graph, node_t** frontier, int head, int tail, int* head_tail);
bool seq_dfs(graph_t* graph, int row, int col);
bool seq_breadth_first_search(graph_t* graph);

//parallel algorithm function prototypes
bool parallel_a_star(graph_t* graph);
int* parallel_bfs_step(graph_t* graph, node_t** frontier, int head, int tail, int* head_tail, node_t*** all_frontiers);
bool parallel_breadth_first_search(graph_t* graph, node_t*** all_frontiers);
bool parallel_dfs(graph_t* graph, int rows, int cols);
double currentSeconds();

void visualize_graph(graph_t* graph){
  // Going through all the nodes
  printf("VISUALIZING\n");
  printf("S is Start\n");
  printf("E is End\n");
  printf("V is Visited\n");
  printf("0 is a regular node\n\n\n");

  printf("%d %d\n", graph->height, graph->width);
  for(int i = 0; i<graph->height; i++){
    for(int j = 0; j<graph->width; j++){
      int t = graph->nodes[i][j]->type;
      if(graph->nodes[i][j]==NULL)
        printf("NULL\n");
      if(j==graph->width-1){
        if(t<10){
          printf("%c\n", t);
        }
        else{
          printf("%c\n", t);
        }
      }
      else{
        if(t<10){
          printf("%c", t);
        }
        else{
          printf("%c", t);
        }
      }
    }
  }
  printf("\n\n\n");
  return;
}

void visualize_graph_node_types(graph_t* graph){
  // Going through all the nodes
  printf("VISUALIZING\n");
  printf("S is Start\n");
  printf("E is End\n");
  printf("V is Visited\n");
  printf("0 is a regular node\n\n\n");

  printf("%d %d\n", graph->height, graph->width);
  for(int i = 0; i<graph->height; i++){
    for(int j = 0; j<graph->width; j++){
      if(graph->nodes[i][j]==NULL)
        printf("NULL\n");
      if(j==graph->width-1){
        printf("%c\n", graph->nodes[i][j]->type);
      }
      else{
        printf("%c",  graph->nodes[i][j]->type);
      }
    }
  }
  printf("\n\n\n");
  return;
}

void visualize_and_write_graph_node_types(graph_t* graph){
  // Going through all the nodes
  FILE *fptr;
  fptr = fopen("vis_out.txt", "w");
  for(int i = 0; i<graph->height; i++){
    for(int j = 0; j<graph->width; j++){
      if(graph->nodes[i][j]==NULL)
        printf("NULL\n");
      if(j==graph->width-1){
        fprintf(fptr, "%c\n", graph->nodes[i][j]->type);
      }
      else{
        fprintf(fptr, "%c",  graph->nodes[i][j]->type);
      }
    }
  }
  fclose(fptr);
  return;
}

void visualize_and_write_graph_node_time(graph_t* graph){
  // Going through all the nodes
  FILE *fptr;
  fptr = fopen("vistime_out.txt", "w");
  for(int i = 0; i<graph->height; i++){
    for(int j = 0; j<graph->width; j++){
      if(graph->nodes[i][j]==NULL)
        printf("NULL\n");
      if(j==graph->width-1){
        fprintf(fptr, "%d\n", graph->nodes[i][j]->time);
      }
      else{
        fprintf(fptr, "%d,",  graph->nodes[i][j]->time);
      }
    }
  }
  fclose(fptr);
  return;
}
graph_t* init_search_space(int width, int height, int *start, int *end){
  printf("Start Init\n");
  graph_t *graph = malloc(sizeof(graph_t));
  graph->nodes = malloc(height * sizeof(node_t***));
  for(int i =0; i<height; i++){
    graph->nodes[i] = malloc(width * sizeof(node_t**));
  }
  // Going through all the nodes
  for(int i = 0; i<height; i++){
    for(int j = 0; j<width; j++){
      graph->nodes[i][j] = malloc(sizeof(node_t));
      graph->nodes[i][j]->x = i;
      graph->nodes[i][j]->y = j;
      graph->nodes[i][j]->time = 0;
      graph->nodes[i][j]->visited = 0;
      if(i==start[0] && j==start[1]){
        graph->nodes[i][j]->type = 'S';
        graph->nodes[i][j]->f = 0;
        graph->nodes[i][j]->g = 0;
        graph->nodes[i][j]->h = 0;
        graph->start = graph->nodes[i][j];
      }
      else if(i==end[0] && j==end[1]){
        graph->nodes[i][j]->type = 'E';
        graph->nodes[i][j]->f = 0;
        graph->nodes[i][j]->g = 0;
        graph->nodes[i][j]->h = 0;
        graph->end = graph->nodes[i][j];
      }
      else{
        graph->nodes[i][j]->type = 'O';
      }
    }
  }
  graph->width = width;
  graph->height = height;
  graph->start_node = start;
  graph->end_node = end;
  if(graph->end==NULL){
    graph->end = graph->nodes[graph->height-1][graph->width-1];
  }

  printf("%d %d\n", graph->height, graph->width);
  //visualize_graph(graph);
  return graph;
}

void free_graph(graph_t *graph, int width, int height){
  for(int i=0; i < height; i++){
    for(int j=0; j < width; j++){
      free(graph->nodes[i][j]);
    }
  }
  for(int k=0; k< height; k++){
    free(graph->nodes[k]);
  }
  free(graph->nodes);
  free(graph);
  return;
}

pqNode1** init_a_queue(graph_t* graph){
  int head = 0;
  pqNode1** a_queue = malloc(sizeof(pqNode1*)*graph->height*graph->width);
  for(int i = 0; i<graph->height; i++){
    for(int j =0; j<graph->width; j++){
      a_queue[head] = malloc(sizeof(pqNode1));
      //a_queue[head]->priority = 0.0;
    }
  }
  return a_queue;
}



node_t*** init_frontiers(graph_t* graph){
  int num_threads = 12;
  node_t*** all_frontiers = malloc(num_threads*sizeof(int*));
  for(int i=0; i<num_threads; i++){
    all_frontiers[i] = malloc(sizeof(node_t*)*graph->width*graph->height);
  }
  return all_frontiers;
}

void add_obstacles(graph_t* graph){
  
  int stride = 10;
  // stride Blocks to the right 
  // stride blocks down 
  int i = 1;
  int j = 0;

  int count = 0;   
  j = 0;
  i = 1; 
  count = 0;
  while(j<graph->width){
    while(count<stride && i<graph->height && j<graph->width){
        if(graph->nodes[i][j]->type!='S' && graph->nodes[i][j]->type!='E'){
          graph->nodes[i][j]->type='B';
          
          count++; 
        }
        j++;
    }
    count = 0; 
    i+=stride; 
  }

  count = 0;
  i = 1;
  j = stride-1;
  while(i<graph->height){
    while(count<stride && i<graph->height && j<graph->width){
      if(graph->nodes[i][j]->type!='S' && graph->nodes[i][j]->type!='E'){
        graph->nodes[i][j]->type='B';
        count++; 
        
      }
      i++;
    }
    count = 0; 
    j+=stride; 
   
  }
  return; 
}



void start_search(int width, int height, int *start, int *end, search_type_t search_type){
  printf("Start Search\n");
  double time_taken;
  graph_t *graph = init_search_space(width, height, start, end);
  switch(search_type) {
    case BFS_SEQ:
    {
      printf("SEQUENTIAL BREADTH FIRST SEARCH\n");
      time_taken = currentSeconds();
      seq_breadth_first_search(graph);
      double total_time_taken = currentSeconds() - time_taken;
      printf("Run 1 Time: %f\n", total_time_taken);
      //visualize_graph(graph);
      //visualize_and_write_graph_node_time(graph);
      break;
    }
    case BFS_PAR:
    {
      node_t*** all_frontiers = init_frontiers(graph);
      printf("PARALLEL BREADTH FIRST SEARCH\n");
      time_taken = currentSeconds();
      parallel_breadth_first_search(graph, all_frontiers);
      double total_time_taken = currentSeconds() - time_taken;
      printf("Run 1 Time: %f\n", total_time_taken);

      //visualize_graph(graph);
      break;
    }
    case DFS_SEQ:
    {
      printf("SEQUENTIAL DEPTH FIRST SEARCH\n");
      printf("Width = %d, Height = %d\n", width, height);
      printf("Start = (%d,%d), End = (%d,%d)\n", start[0], start[1], end[0], end[1]);
      time_taken = currentSeconds();
      seq_dfs(graph, graph->start_node[0], graph->start_node[0]);
      time_taken = currentSeconds() - time_taken;
      printf("Run 1 Time: %f\n", time_taken);
      //visualize_graph_node_types(graph);
      //visualize_and_write_graph_node_time(graph); 
      free_graph(graph, width, height);

      /*graph = init_search_space(width, height, start, end);
      //visualize_graph(graph);
      time_taken = currentSeconds();
      seq_dfs(graph, height, width);
      //visualize_graph(graph);
      time_taken = currentSeconds() - time_taken;
      free_graph(graph, width, height);
      printf("Run 2 Time: %f\t", time_taken);

      graph = init_search_space(width, height, start, end);
      //visualize_graph(graph);
      time_taken = currentSeconds();
      seq_dfs(graph, height, width);
      //visualize_graph(graph);
      time_taken = currentSeconds() - time_taken;
      free_graph(graph, width, height);
      printf("Run 3 Time: %f\n", time_taken);*/ 
      break;
    }
    case DFS_PAR:
    {
      printf("PARALLEL DEPTH FIRST SEARCH\n");
      printf("Width = %d, Height = %d\n", width, height);
      printf("Start = (%d,%d), End = (%d,%d)\n", start[0], start[1], end[0], end[1]);

      time_taken = currentSeconds();
      parallel_dfs(graph, height, width);
      time_taken = currentSeconds() - time_taken;
      printf("Run 1 Time: %f\n", time_taken);
      //visualize_graph_node_types(graph)
      free_graph(graph, width, height);
      

      /*graph = init_search_space(width, height, start, end);
      time_taken = currentSeconds();
      parallel_dfs(graph, height, width);
      time_taken = currentSeconds() - time_taken;
      printf("Run 2 Time: %f\t", time_taken);
      free_graph(graph, width, height);
      

      graph = init_search_space(width, height, start, end);
      time_taken = currentSeconds();
      parallel_dfs(graph, height, width);
      time_taken = currentSeconds() - time_taken;
      printf("Run 3 Time: %f\n", time_taken);
      free_graph(graph, width, height);*/
      
      break;
    }
    case A_SEQ:
    {
      //add_obstacles(graph);
      printf("SEQUENTIAL A*  SEARCH\n");
      time_taken = currentSeconds();
      a_star(graph);
      double total_time_taken = currentSeconds() - time_taken;
      printf("Run 1 Time: %f\n", total_time_taken);
      //visualize_graph_node_types(graph);
      //visualize_and_write_graph_node_types(graph); 
      //visualize_and_write_graph_node_time(graph); 
      break;
    }
    case A_PAR:
    {
      //add_obstacles(graph);
      printf("Parallel A* SEARCH\n");
      time_taken = currentSeconds();
      parallel_a_star(graph);
      double total_time_taken = currentSeconds() - time_taken;
      printf("Run 1 Time: %f\n", total_time_taken);
      //visualize_graph_node_types(graph);
      //visualize_and_write_graph_node_types(graph); 
      //visualize_and_write_graph_node_time(graph); 
      break;
    }
  }
}
