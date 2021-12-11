// server.h
// declares the server.

#ifndef SERVER_H
#define SERVER_H

// socket configs
#define SOCKET_PROTOCAL 0
#define SOCKET_BACKLOG_SIZE 255

// argument indices
typedef enum argument_index
{
    PROGRAM_NAME,
    ARGUMENT_FILE_NAME,
    PORT_NUMBER,
    EXPECTED_ARGC
} argument_index_t;

// declares error codes and error code handling protocol.

// error codes
typedef enum error_code
{
    SUCCESS,
    INVALID_ARGUMENT,      // invalid number of arguments.
    FAILED_TO_OPEN_ARGUMENT,  // failed to open the file specified by the argument for read.
    EMPTY_ARGUMENT_FILE,    // empty file
    FAILED_TO_OPEN_OUTPUT, // failed to open the first file for write.
    FAILED_TO_OPEN_INPUT,  // failed to open one of the subsequent files for read.
    OUT_OF_MEMORY,    // out of memory
    ERR_HOST_NAME,
    ERR_SOCKET,
    ERR_BIND,
    ERR_LISTEN,
    ERR_ACCEPT,
    ERR_OPEN,
} error_code_t;

/// print out the error.
/// @param:
/// - error_code: the error code of the error to print out.
/// - message: any additional to print out, NULL if no additional message.
/// @return: returns the error code.
unsigned int error_handler(unsigned int error_code, char *message);

/// main
int main(int argc, char* argv[]);

#endif /* SERVER_H */
