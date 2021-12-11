// server.h
// declares the server.

#ifndef SERVER_H
#define SERVER_H

// socket configs
#define SOCKET_PROTOCAL 0
#define SOCKET_BACKLOG_SIZE 255
#define EVENT_SIZE 5

// argument indices
typedef enum argument_index
{
    PROGRAM_NAME,
    ARGUMENT_FILE_NAME,
    PORT_NUMBER,
    EXPECTED_ARGC
} argument_index_t;

#endif /* SERVER_H */
