#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_LENGTH 100

int main(void) {
    int pfd[2], cid, tmp, len;
    // File* rdstream, *wtstream;
    char mesgs[MAX_LENGTH];
    memset(mesgs, 0, sizeof(mesgs));
    pipe(pfd);

    if ((cid = fork()) == -1) {
        //creating process fails...
        printf("fork failed...\n");
    } else if (cid == 0) {
        //child
        if ((tmp = close(pfd[0])) == -1) {
            printf("child close failed");
        } else {
            char *sending_msg = "this message comes from the child process";
            len = strlen(sending_msg) + 1;
            write(pfd[1], sending_msg, len);
            close(pfd[1]);
        }
        exit(0);
    } else {
        //parent
        if ((tmp = close(pfd[1])) == -1) {
            printf("parent close failed");
        } else {
            wait(NULL);
            read(pfd[0], mesgs, MAX_LENGTH);
            // write(1, mesgs, MAX_LENGTH);
            printf("read from pipe: %s\n", mesgs);
            close(pfd[0]);
        }
    }

    return 0;
}