# Parallel-Search

Steps to run:
- Set the height and width of the graph in main.c 
- Set the start and end of the graph in main.c 
- Select one of DFS_SEQ, DFS_PAR, BFS_SEQ, BFS_PAR, A_SEQ, A_PAR as the search algorithm in main.c 
- make all on command line 
- ./output to view the results 

Overview of Some of the Results:

Sequential A* visualisation
<img src="https://github.com/ishaan-jaff/Parallel-Search/blob/master/visualisation_code/Screen%20Shot%202020-05-01%20at%203.13.20%20PM.png" width="250" height="250">

Parallel A*, 7x speedup (over sequential A*) for 10^6 nodes and 137x speedup (over sequential A*) for 10^8 nodes 
<img src="https://github.com/ishaan-jaff/Parallel-Search/blob/master/visualisation_code/Screen%20Shot%202020-05-01%20at%201.39.55%20PM.png" width="250" height="250">




