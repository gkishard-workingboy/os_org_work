// server.h
// declares the server.

#ifndef SERVER_H
#define SERVER_H

// argument indices
typedef enum argument_index
{
    PROGRAM_NAME,
    ARGUMENT_FILE_NAME,
    PORT_NUMBER,
    EXPECTED_ARGC
} argument_index_t;

// socket configs
#define SOCKET_PROTOCAL 0
#define SOCKET_BACKLOG_SIZE 10
#define EVENT_SIZE 5

#endif /* SERVER_H */
