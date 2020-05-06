#include <stdio.h>
#include <stdlib.h>
#include "util.h"

void start_search(int width, int height, int *start, int *end, search_type_t search_type);

int main() {
    int width = 50;
    int height = 50;
    int start[2] = {0,0};
    int end[2] = {49,49};
    start_search(width, height, start, end, A_PAR);
    return 0;
}
