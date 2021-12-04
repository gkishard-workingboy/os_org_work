#include <sys/select.h>
#include <sys/time.h>
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
First, create a socket that will listen for new connections from clients with the socket(),
 bind(), and listen() system calls, as described in the Linux Pipes, FIFOs, and Sockets studio.
Your program should use domain AF_INET so that it can work remotely. We will still use the 
connection type SOCK_STREAM, and protocol zero. If you want your server to listen at a specific 
IP address, you can use the ifconfig command from a terminal window, to see the list of network 
interfaces enabled in the current machine and choose one. Your client should use that same IP 
address and port to connect to the server.


Second, in a loop, watch both the standard input stream (STDIN_FILENO) and the server's listening 
socket via the select() system call. For both of them, we will only watch for read events. The 
calls to select() should block until an event occurs and you should pass NULL as the last argument 
for select(). If data from the standard input stream is ready, read and print it to the screen as 
before. If there is a connection request from a client, select() will also return: in this case, 
your program should accept the client's connection0 request by calling the accept() system call. 
This invocation of accept() should return immediately (without blocking). Your server should send 
a message (for example, it may include the server's hostname, the current time on the server's 
machine, etc.) to the client, and then close the connection.
*/


int main(void) {
    int server_fd = -1;
    int con_fd = 0;
    struct sockaddr_in socket_address, peer_address;
    socklen_t listen_socket_len, peer_socket_len;
    int opt = 1;
    int ready, fd, nfds, still_looping = 1, hostp = 0;
    ssize_t num_bytes;
    fd_set rdset;
    char buf[BUFSIZE];
    char hostname[BUFSIZE];
    char hosttime[BUFSIZE];
    time_t now;
    struct tm *timeinfo;
    
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket creating failed");
        exit(EXIT_FAILURE);
    }

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


    strcpy(hostname, "hostname: ");
    hostp = strlen(hostname);
    if (gethostname(hostname + hostp, BUFSIZE) < 0) {
        perror("get hostname failed");
        exit(EXIT_FAILURE);
    }
    hostp = strlen(hostname);
    hostname[hostp] = '\n';
    hostname[hostp+1] = 0;
    strcpy(hosttime, "the current time is ");
    nfds = server_fd;

    // while loop for accept socket connections.
    do {
        FD_ZERO(&rdset);
        FD_SET(STDIN_FILENO, &rdset);
        FD_SET(server_fd, &rdset);
        if (con_fd > 0) {
            FD_SET(con_fd, &rdset);
        }
        if (con_fd > nfds) {
            nfds = con_fd;
        }
        if ((ready = select(nfds + 1, &rdset, NULL, NULL, NULL)) > 0) {
            if (ready < 0 && errno != EINTR) {
                perror ("select error");
            }
            for (fd = 0; fd <= nfds; ++fd) {
                if (FD_ISSET(fd, &rdset)) {
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
                        hostp = strlen(hosttime);
                        strcpy(hosttime + hostp, asctime(timeinfo));
                        write(con_fd, hostname, strlen(hostname));
                        write(con_fd, hosttime, strlen(hosttime));
                    }else {
                        num_bytes = read(fd, buf, BUFSIZE);
                        if (num_bytes == -1) {
                            fprintf(stderr, "Reading from file descriptor %d is failed.\n", fd);
                            break;
                        }

                        // peer just disconnected.
                        if (num_bytes == 0) {
                            printf("client disconnected, %s:%d\n", inet_ntoa(peer_address.sin_addr), ntohs(peer_address.sin_port));
                            close(con_fd);
                            con_fd = 0;
                            break;
                        }

                        //null terminator to replace the \n
                        buf[num_bytes] = 0;
                        if (strcmp(buf, "quit\n") == 0) {
                            still_looping = 0;
                            break;
                        }
                        printf("-> %s \n", buf);
                    }
                }
            }
        }
    }
    while (still_looping);

    //close passive socket
    if (server_fd >= 0) {
        close(server_fd);
    }

    return 0;
}