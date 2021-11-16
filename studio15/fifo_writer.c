#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include "test_fifo.h"

int main(int argc, char **argv) {
    FILE *fwriter;
    char *fifoname = FIFO_NAME;
    if (argc < 2) {
        printf("Usage: fifo_writer [start_num]\n");
        return 0;
    }
    int start = atoi(argv[1]);
    printf("Start: %d\n", start);
    fwriter = fopen(fifoname, "w");

    for (int i = start; i < INT_MAX; i += 2) { 
        fprintf(fwriter, "%d ", i);
    }

    return 0;
}