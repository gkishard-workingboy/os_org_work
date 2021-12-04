#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include "socket_macro.h"

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
    int cli_fd, rd_size;
    struct sockaddr_in server_addr;
    char buffer[BUFSIZE];
    char *msg = "hello~\nthis message comes from me -- an anonymous client.\nhope you can receive this message completely.";

    if ((cli_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket creating failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Invalid address\n");
        exit(EXIT_FAILURE);
    }

    if (connect(cli_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connection failed");
    } else {
        rd_size = read(cli_fd, buffer, BUFSIZE);
        buffer[rd_size] = 0;
        printf("%s", buffer);
        rd_size = read(cli_fd, buffer, BUFSIZE);
        buffer[rd_size] = 0;
        printf("%s", buffer);
        write(cli_fd, msg, strlen(msg));
        printf("message is sent\n");
    }
    //close socket
    close(cli_fd);

    return 0;
}