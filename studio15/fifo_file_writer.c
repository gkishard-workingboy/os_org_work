#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "test_fifo.h"

int main(int argc, char **argv) {
    char *fifoname = FIFO_NAME;
    char *buf;
    FILE *fwriter, *freader;
    ssize_t nread;
    size_t len = 0;

    if (argc != 2) {
        printf("Usage: %s <file>\n", argv[0]);
        exit(1);
    }

    freader = fopen(argv[1], "r");
    if (freader == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    fwriter = fopen(fifoname, "w");

    while ((nread = getline(&buf, &len, freader)) != -1) {
        fprintf(fwriter, "%s", buf);
    }

    free(buf);
    fclose(freader);
    fclose(fwriter);
    return 0;
}