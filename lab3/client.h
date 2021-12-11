// client.h
// declares the client.

#ifndef CLIENT_H
#define CLIENT_H

// socket configs
#define SOCKET_PROTOCAL 0

// argument indices
typedef enum argument_index
{
    PROGRAM_NAME,
    HOST_NAME,
    PORT_NUMBER,
    EXPECTED_ARGC
} argument_index_t;

// declares error codes and error code handling protocol.

// error codes
typedef enum error_code
{
    SUCCESS,
    INVALID_ARGUMENT,      // invalid number of arguments.
    ERR_HOST_NAME,
    ERR_SOCKET,
    ERR_CONNECT,
} error_code_t;

/// print out the error.
/// @param:
/// - error_code: the error code of the error to print out.
/// - message: any additional to print out, NULL if no additional message.
/// @return: returns the error code.
unsigned int error_handler(unsigned int error_code, char *message);

/// main
int main(int argc, char* argv[]);

#endif /* CLIENT_H */
