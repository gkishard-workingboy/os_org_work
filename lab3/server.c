// server.c
// defines the server.

#include "server.h"
#include "error.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

// defines the maximum number of characters in a file name.
// 255 for linux.
#define NAME_MAX 255

int main(int argc, char* argv[])
{
    // stores the return value of a function call
    // int ret;
    // stores the file name.
    char file_name[NAME_MAX+1];
    // stores a file pointer to the initial input file specified by the argument.
    FILE *argument;
    // stores a file pointer to the output file specified by the initial input file.
    FILE *output;
    // stores an array of file pointers to the input files specified by the initial input file.
    // allocated dynamically on the heap at runtime.
    FILE **inputs;

    // validate arguments
    if (argc != EXPECTED_ARGC)
    {
        // invalid number of arguments
        return error_handler(INVALID_ARGUMENT, argv[PROGRAM_NAME]);
    }

    // open initial file for read
    argument = fopen(argv[ARGUMENT_FILE_NAME], "r");
    if (argument == NULL)
    {
        // failed to open file.
        return error_handler(FAILED_TO_OPEN_ARGUMENT, strerror(errno));
    }

    // open output file for write
    // use 255 as max.
    fscanf(argument, "%255s", file_name);
    printf("%s\n", file_name);

    // free(inputs);

    fclose(argument);

    return SUCCESS;
}