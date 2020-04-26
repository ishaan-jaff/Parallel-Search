#include <stdio.h>
#include <stdlib.h>
#include "util.h"

void start_search(int width, int height, int *start, int *end, search_type_t search_type);

int main() {
    int width = 1000;
    int height = 1000;
    int start[2] = {0,0};
    int end[2] = {999,999};
    //start_search(width, height, start, end, DFS_SEQ);
    start_search(width, height, start, end, DFS_PAR);
    return 0;
}
