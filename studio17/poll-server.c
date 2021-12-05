#define _GNU_SOURCE 1
#include <poll.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "socket_macro.h"

/*
Update the new copy of your server code to use poll() as its I/O multiplexer instead of 
select() (please see pages 1340-1341 of the LPI text book for an illustration of how to 
use poll()). For read events from the standard input stream, the socket on which it accepts 
connections, or a connected client socket, the server should use POLLIN, and for disconnection 
events on the client socket the server should use POLLRDHUP (note that in order to use 
POLLRDHUP, your program will need to say #define _GNU_SOURCE before it includes any header files).

A straightforward way to manage the pollfd structs for these event sources is to put them 
into an array with the fd and events for the standard input stream at position 0, the socket 
on which the server listens for (and accepts) connection requests at position 1 (if it is 
successfully established), and the fd and events for a connected client socket at position 2. 
Then, you can use an integer variable (which you also will pass into the call to poll()) 
to indicate how many (and thus, because of their ordering in the array, which) of these 
event sources are active - it will start off with value 1 since the standard input stream 
is always available; it will increase to 2 once the socket(), bind(), and listen() calls 
have succeeded, and then it will increase to 3 when a client socket connection is successfully 
established and decrease to 2 when that connection is closed.

Modify your server code so that after it accepts a connection from a client, instead of sending 
out a message and closing the connection, it adds the file descriptor returned from the accept() 
system call to the watched list for input events. Your server should then use poll() to wait to 
read data from the client, store the data when it is ready and has been read, and print it to 
the screen when a specific delimiter is reached (for example, the line break character). Your 
server could print the delimiter-separated messages one by one as it receives them from the client.

Modify your server code, so that when it reads the message "quit" from a client (or from the 
standard input stream as before), it terminates the program. Your server should close all sockets 
and files before it terminates.

Modify your server code so that if it receives a POLLRDHUP event on the client connection, it 
will close the socket and then decrement the number of fds on which it will poll (leaving only 
the fds for the standard input stream and the socket on which it accepts connections active).

Modify your client so that once it connects it sends multiple messages to the server (the 
messages should end with the appropriate delimiters so that the server knows when to print 
them out), and then closes the connection.

Because either (1) POLLRDHUP or (2) POLLIN or (3) both or (4) neither of them may be enabled 
for the client socket fd when poll() returns, it is important that your code handle all four 
of these cases correctly. One straightforward way to do this is to check first whether either 
of them is enabled (POLLRDHUP | POLLIN) and then within that branch of the code check for 
(and handle) each of them separately.
*/


int main(void) {
    int server_fd = -1;
    int con_fd = 0;
    struct sockaddr_in socket_address, peer_address;
    socklen_t listen_socket_len, peer_socket_len;
    int opt = 1;
    int ready, fd, still_looping = 1, hostp = 0;
    ssize_t num_bytes;
    unsigned int nfds = 0;
    char buf[BUFSIZE];
    char hostmsg[BUFSIZE];
    time_t now;
    struct tm *timeinfo;
    struct pollfd fds[FDSSIZE];
    
    fds[nfds].fd = STDIN_FILENO;
    fds[nfds].events = POLLIN;
    nfds++;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket creating failed");
        exit(EXIT_FAILURE);
    }

    fds[nfds].fd = server_fd;
    fds[nfds].events = POLLIN;
    nfds++;

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        perror("setsocketopt failed");
        exit(EXIT_FAILURE);
    }

    memset(&socket_address, 0, sizeof(struct sockaddr_in));
    socket_address.sin_family = AF_INET;
    socket_address.sin_addr.s_addr = INADDR_ANY;
    socket_address.sin_port = htons(PORT);
    
    if (bind(server_fd, (struct sockaddr *)&socket_address, sizeof(socket_address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    //listen to specified socket file descriptor, the backlog is 1, which means it only allows one 
    //connection at one time.
    if (listen(server_fd, 1) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    // prepare the message sent to the client
    strcpy(hostmsg, "hostname: ");
    hostp = strlen(hostmsg);
    if (gethostname(hostmsg + hostp, BUFSIZE) < 0) {
        perror("get hostname failed");
        exit(EXIT_FAILURE);
    }
    hostp = strlen(hostmsg);
    hostmsg[hostp] = '\n';
    strcpy(hostmsg + hostp + 1, "the current time is ");
    hostp = strlen(hostmsg);


    // while loop for accept socket connections.
    do {
        //poll will block untill some fd is ready
        if ((ready = poll(fds, nfds, -1)) > 0) {
            if (ready < 0 && errno != EINTR) {
                perror ("poll error");
            }
            for (int i = 0; i < nfds; ++i) {
                fd = fds[i].fd;
                // peer disconnected.
                if (fds[i].revents & POLLRDHUP) {
                    // check if there is some data available.
                    while ((num_bytes = read(fd, buf, BUFSIZE)) > 0) {
                        buf[num_bytes] = 0;
                        if (strcmp(buf, "quit\n") == 0) {
                            still_looping = 0;
                            break;
                        }
                        printf("-> %s", buf);
                    }
                    printf("client disconnected, %s:%d\n", inet_ntoa(peer_address.sin_addr), ntohs(peer_address.sin_port));
                    close(fd);
                    nfds--;
                } else if (fds[i].revents & POLLIN) {// input is ready
                    //new connection.
                    if (fd == server_fd) {
                        if ((con_fd = accept(server_fd, (struct sockaddr *)&peer_address, &peer_socket_len)) < 0) {
                            perror("accept failed");
                            exit(EXIT_FAILURE);
                        }
                        printf("incoming connection socket: %s:%d\n", inet_ntoa(peer_address.sin_addr), ntohs(peer_address.sin_port));
                        time(&now);
                        timeinfo = localtime(&now);
                        printf("current time: %s", asctime(timeinfo));
                        printf("connection file descriptor: %d\n", con_fd);
                        strcpy(hostmsg + hostp, asctime(timeinfo));
                        write(con_fd, hostmsg, strlen(hostmsg));
                        // register the new socket into poll
                        fds[nfds].fd = con_fd;
                        fds[nfds].events = (POLLRDHUP | POLLIN);
                        nfds++;
                    }else {
                        //read from ready fd.
                        num_bytes = read(fd, buf, BUFSIZE);
                        if (num_bytes == -1) {
                            fprintf(stderr, "Reading from file descriptor %d is failed.\n", fd);
                            break;
                        }
                        //null terminator to replace the \n
                        buf[num_bytes] = 0;
                        if (strcmp(buf, "quit\n") == 0) {
                            still_looping = 0;
                            break;
                        }
                        printf("-> %s", buf);
                    }
                }
            }
        }
    }
    while (still_looping);

    //close passive socket
    for (int i = 1; i < nfds; ++i) {
        fd = fds[i].fd;
        if (fd > 1) {
            close(fd);
        }
    }

    return 0;
}