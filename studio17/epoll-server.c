#include <sys/epoll.h>
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


int main(void) {
    int server_fd = -1, epoll_fd = -1;
    int con_fd = 0;
    struct sockaddr_in socket_address, peer_address;
    socklen_t listen_socket_len, peer_socket_len;
    int opt = 1;
    int ready, fd, still_looping = 1, hostp = 0, nevs = 0;
    ssize_t num_bytes;
    char buf[BUFSIZE];
    char hostmsg[BUFSIZE];
    time_t now;
    struct tm *timeinfo;
    struct epoll_event ev, evlist[EVENTSIZE];
    
    if ((epoll_fd = epoll_create1(0)) == -1) {
        perror("create epoll failed");
        exit(EXIT_FAILURE);
    }

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket creating failed");
        exit(EXIT_FAILURE);
    }

    ev.data.fd = STDIN_FILENO;
    ev.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &ev) == -1) {
        perror("epoll_ctl failed");
        exit(EXIT_FAILURE);
    }

    ev.data.fd = server_fd;
    ev.events = EPOLLIN;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev) == -1) {
        perror("epoll_ctl failed");
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
        //epoll will block
        if ((ready = epoll_wait(epoll_fd, evlist, EVENTSIZE, -1)) > 0) {
            if (ready < 0 && errno != EINTR) {
                perror ("poll error");
            }
            for (int i = 0; i < ready; i++) {
                fd = evlist[i].data.fd;
                if (evlist[i].events & EPOLLIN) {
                    // input is ready
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
                        // register the new socket into epoll
                        ev.events = EPOLLIN | EPOLLRDHUP;
                        ev.data.fd = con_fd;
                        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, con_fd, &ev);
                    }else {
                        // peer disconnected.
                        if (evlist[i].events & EPOLLRDHUP) {
                            // check if there is some data available.
                            // while ((num_bytes = read(fd, buf, BUFSIZE)) > 0) {
                            //     buf[num_bytes] = 0;
                            //     if (strcmp(buf, "quit\n") == 0) {
                            //         still_looping = 0;
                            //         break;
                            //     }
                            //     printf("-> %s", buf);
                            // }
                            // printf("client disconnected, %s:%d\n", inet_ntoa(peer_address.sin_addr), ntohs(peer_address.sin_port));
                            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                            close(fd);
                        }
                        printf("data available!\n");
                        //read from ready fd.
                        // num_bytes = read(fd, buf, BUFSIZE);
                        // if (num_bytes == -1) {
                        //     fprintf(stderr, "Reading from file descriptor %d is failed.\n", fd);
                        //     break;
                        // }
                        //null terminator to replace the \n
                        // buf[num_bytes] = 0;
                        // if (strcmp(buf, "quit\n") == 0) {
                        //     still_looping = 0;
                        //     break;
                        // }
                        // printf("-> %s", buf);
                    }
                }
            }
        }
    }
    while (still_looping);

    //close passive socket
    close(con_fd);
    close(server_fd);
    close(epoll_fd);



    return 0;
}