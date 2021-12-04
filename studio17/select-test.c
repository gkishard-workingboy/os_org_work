#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define BUFSIZE 128

// In this exercise we will use the select() system call with a single file descriptor, 
// for the standard input stream. Even though handling just a single file descriptor 
// isn't what select() is designed for, this helps us get acquainted with this system 
// call. We will write a program that uses select() to watch for data (i.e., input from the keyboard) 
// from the standard input stream (STDIN_FILENO). When the standard input stream becomes 
// ready, your program will read the data and print it out to the screen. Your program 
// should repeatedly invoke select() to watch for new input and only exit when receives 
// the string "quit" from the standard input stream (note that you may need to have your 
// code ignore additional characters after that string, in order to make this work correctly). 
// Page 1335 of the LPI text book (Kerrisk) provides an example of how to use select(), 
// which may be helpful to get started on this exercise.
// Note: The file descriptor sets that you pass to select() may be modified after it returns. 
// Thus, you should keep extra unchanged copies of them and/or (re)initialize them before every 
// invocation of select().



int main(void) {
    int ready, fd, nfds, still_looping = 1;
    ssize_t num_bytes;
    fd_set rdset;
    char buf[BUFSIZE];
    nfds = STDIN_FILENO + 1;
    do {
        FD_ZERO(&rdset);
        FD_SET(STDIN_FILENO, &rdset);
        if ((ready = select(1, &rdset, NULL, NULL, NULL)) > 0) {
            for (fd = 0; fd < nfds; ++fd) {
                if (FD_ISSET(fd, &rdset)) {
                    num_bytes = read(fd, buf, BUFSIZE);
                    if (num_bytes < 1) {
                        fprintf(stderr, "Reading from file descriptor %d is failed.", fd);
                        break;
                    }
                    //null terminator to replace the \n
                    buf[num_bytes-1] = 0;
                    if (strcmp(buf, "quit") == 0) {
                        still_looping = 0;
                        break;
                    }
                    printf("Read from stdin: %s \n", buf);
                }
            }
        }
    }
    while (still_looping);


    return 0;
}