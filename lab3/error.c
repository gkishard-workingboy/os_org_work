// error.c
// defines error code handling protocol.

#include "error.h"

#include <stdio.h>

// error messages
static const char *error_message[] = {
    "Success",
    "Invalid argument(s)",
    "Failed to open argument file for read",
    "Failed to open output file for write",
    "Failed to open input file for read",
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