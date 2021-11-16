#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include "test_fifo.h"

int main(void) {
    FILE *freader, *fwriter;
    char buf[MAX_LENGTH];
    int result;
    char *fifoname = FIFO_NAME;
    mkfifo(fifoname, S_IRUSR | S_IWUSR);

    freader = fopen(fifoname, "r");
    fwriter = fopen(fifoname, "w");

    //section 3
    // while (fgets(buf, MAX_LENGTH, freader) != NULL) {
    //     printf("%s", buf);
    // }

    //section 4
    // while (fscanf(freader, "%d", &result) != EOF) {
    //     printf("origin:%d doubled:%d\n", result, 2*result);
    // }

    //section 5
    while (fgets(buf, MAX_LENGTH, freader) != NULL) {
        printf("%s", buf);
    }
    
    return 0;
}