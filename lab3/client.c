// client.c
// defines the client.

#include "client.h"
// utils
#include <stdlib.h>
#include <string.h>
#include <errno.h>
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


// defines error code handling protocol.
// error messages
static const char* error_message[] =
{
	"Success",
	"Unexpected number of arguments",
	"Failed to get host name",
	"Failed to create data socket",
	"Failed to connect socket",
	"Failed to write to socket",
	"Failed to open data socket",
};

/// print out the error.
/// @param:
/// - error_code: the error code of the error to print out.
/// - message: any additional to print out, NULL if no additional message.
/// @return: returns the error code.
unsigned int error_handler(unsigned int error_code, char *message)
{
    if (error_code == INVALID_ARGUMENT)
    {
        // invalid argument.
        fprintf(stderr, "%s\n", error_message[error_code]);
        // print usage message.
        if (message == NULL)
        {
            // no additional message, program fault.
            fprintf(stderr, "Usage: [program] [file] [port]\n");
        }
        else
        {
            // additional message.
            fprintf(stderr, "Usage: %s [file] [port]\n", message);
        }
    }
    else {
        // program error, print error message.
        if (message == NULL) {
            // no additional message.
            fprintf(stderr, "%s\n", error_message[error_code]);
        } else
        {
            // additional message.
            fprintf(stderr, "%s: %s\n", error_message[error_code], message);
        }
    }
    return error_code;
}

/// main
int main(int argc, char* argv[])
{
    /* variables */
    
    // stores the return value of system calls.
    int ret;

    /* body */

    // validate arguments
    if (argc != EXPECTED_ARGC)
    {
        // invalid number of arguments
        return error_handler(INVALID_ARGUMENT, argv[PROGRAM_NAME]);
    }

    return SUCCESS;
}
