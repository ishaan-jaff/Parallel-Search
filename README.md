# Parallel-Search

Steps to run:
- Set the height and width of the graph in main.c 
- Set the start and end of the graph in main.c 
- Select one of DFS_SEQ, DFS_PAR, BFS_SEQ, BFS_PAR, A_SEQ, A_PAR as the search algorithm in main.c 
- make all on command line 
- ./output to view the results 

## Overview of Some of the Results:
Run on Eight-core, 3.0 GHz Intel Core i7 processor<br/>


### Sequential A* visualisation

<style>
.padding-table-columns td
{
    padding:0 5px 0 0; /* Only right padding*/
}
  </style>

<table className="padding-table-columns">
  <tr>
    <td>First Screen Page</td>
     <td>Holiday Mention</td>
  </tr>
  <tr>
    <td><img src="https://github.com/ishaan-jaff/Parallel-Search/blob/master/visualisation_code/Screen%20Shot%202020-05-01%20at%203.13.20%20PM.png" width="250" height="250"></td>
    <td><img src="https://github.com/ishaan-jaff/Parallel-Search/blob/master/visualisation_code/Screen%20Shot%202020-05-01%20at%201.39.55%20PM.png" width="250" height="250"></td>
  </tr>
 </table>




### Parallel A*, 7x speedup (over sequential A*) for 10^6 nodes and 137x speedup (over sequential A*) for 10^8 nodes
<img src="https://github.com/ishaan-jaff/Parallel-Search/blob/master/visualisation_code/Screen%20Shot%202020-05-01%20at%201.39.55%20PM.png" width="250" height="250">
<br/>




### Sequential Breadth First Search




<img src="https://github.com/ishaan-jaff/Parallel-Search/blob/master/visualisation_code/Screen%20Shot%202020-05-01%20at%203.07.21%20PM.png" width="250" height="250">
<br/>


### Parallel Breadth First Search (all speedups compared to sequential version), 2.7x speedup for 10^8 nodes, 3x speedup for 10^7 nodes, 1.3x speedup for 10^6 nodes




<img src="https://github.com/ishaan-jaff/Parallel-Search/blob/master/visualisation_code/Screen%20Shot%202020-05-01%20at%201.44.22%20PM.png" width="250" height="250">
<br/>


