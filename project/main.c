#include <stdio.h>
#include <stdlib.h>
#include "util.h"

void start_search(int width, int height, int *start, int *end, search_type_t search_type);

int main() {
    int width1 = 20;
    int height1 = 20;
    int width2 = 1000;
    int height2 = 10000;
    int width3 = 10000;
    int height3 = 1000;
    int width4 = 10000;
    int height4 = 10000;
    int start[2] = {0,0};
    int end[2] = {1,0};
/*
    start_search(width1, height1, start, end, DFS_SEQ);
    start_search(width2, height2, start, end, DFS_SEQ);
    start_search(width3, height3, start, end, DFS_SEQ);
    start_search(width4, height4, start, end, DFS_SEQ);
*/

    start_search(width1, height1, start, end, DFS_PAR);
    //start_search(width2, height2, start, end, DFS_PAR);
    //start_search(width3, height3, start, end, DFS_PAR);
    //start_search(width4, height4, start, end, DFS_PAR);
    return 0;
}
