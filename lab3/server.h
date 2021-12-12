// server.h
// declares the server.

#ifndef SERVER_H
#define SERVER_H

// uses GNU extension reallocarray() and fcloseall()
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
// for socket(), bind()
#include <sys/types.h>
#include <sys/socket.h>
// for unix
#include <sys/un.h>
// for remote
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <netdb.h>
//for epoll
#include <sys/epoll.h>

// socket configs
#define SOCKET_PROTOCAL 0
#define SOCKET_BACKLOG_SIZE 255
#define EVENT_SIZE 5
#define RD_BUF_SIZE 1024
#define STOP "-1 \n"

// argument indices
typedef enum argument_index
{
    PROGRAM_NAME,
    ARGUMENT_FILE_NAME,
    PORT_NUMBER,
    EXPECTED_ARGC
} argument_index_t;

// read write object
typedef struct read_write_object {
    unsigned int last_len;
    unsigned int line_len;
    char* line_buf;
    FILE* fptr;
} rw_obj;

#endif /* SERVER_H */
