// error.c
// defines error code handling protocol.

#include "error.h"

#include <stdio.h>

// error messages
static const char *error_message[] = {
    "Success",
    "Invalid argument(s)",
    "Failed to open argument file for read",
    "Empty file",
    "Failed to open output file for write",
    "Failed to open input file for read",
    "Out of memory",
	"Failed to get host name",
	"Failed to create connection socket",
	"Failed to bind socket",
	"Failed to listen to socket",
	"Failed to accept data socket",
	"Failed to open data socket",
    "Failed to create epoll",
    "Failed to control epoll",
};

// print out the error.
// @param:
// - error_code: the error code of the error to print out.
// - message: any additional to print out, NULL if no additional message.
// @return: returns the error code.
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
            fprintf(stderr, "Usage: lab3 [file] [port]\n");
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

unsigned int error_helper(unsigned int error_code, char* message, void* arg1, void *arg2) {
    free(arg1);
    free(arg2);
    return error_handler(error_code, NULL);
}
