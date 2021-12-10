// server.c
// defines the server.

#include "server.h"
#include "error.h"

#define _GNU_SOURCE
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
    int ret;
    
    // stores the file name.
    char file_name[NAME_MAX+1];
    
    // stores a file pointer to the initial input file specified by the argument.
    FILE *argument;
    
    // stores a file pointer to the output file specified by the initial input file.
    FILE *output;
    
    // stores an array of file pointers to the input files specified by the initial input file.
    // allocated dynamically on the heap at runtime.
    FILE **inputs;
    FILE **new;
    // inputs size.
    size_t inputs_size = 0;
    // max inputs capacity.
    size_t inputs_capacity = 4;
    
	// read from file
	unsigned int current_len = 0;
	unsigned int max_len = 0;
	char* line_buf = NULL;

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

    // scan output file name
    ret = fscanf(argument, "%s", file_name);
    if (ret < SUCCESS)
    {
        // empty file.
        return error_handler(EMPTY_ARGUMENT_FILE, file_name);
    }
    // open output file for write
    output = fopen(file_name, "w");
    if (output == NULL)
    {
        // failed to open file.
        fcloseall();
        return error_handler(FAILED_TO_OPEN_OUTPUT, strerror(errno));
    }

    // allocate inputs
    inputs = calloc(inputs_capacity, sizeof(FILE*));
    if (inputs == NULL) {
        // calloc failed.
        fcloseall();
        return error_handler(OUT_OF_MEMORY, NULL);
    }
    // scan next input file name
    ret = fscanf(argument, "%s", file_name);
    while (ret >= SUCCESS) {
        // success, increment size.
        inputs_size = inputs_size + 1;
        // check if need to allocate new memory.
        if (inputs_size > inputs_capacity)
        {
            // reallocate inputs to be twice as big.
            inputs_capacity = inputs_capacity * 2; // optimize to be << 1.
            new = reallocarray(inputs, inputs_capacity, sizeof(FILE*));
            free(inputs);
            if (new == NULL)
            {
                // reallocarray failed.
                fcloseall();
                return error_handler(OUT_OF_MEMORY, NULL);
            }
            free(inputs);
            inputs = new;
        }
        // open input file for read
        inputs[inputs_size-1] = fopen(file_name, "r");
        if (inputs[inputs_size-1] == NULL)
        {
            // failed to open file.
            free(inputs);
            return error_handler(FAILED_TO_OPEN_INPUT, strerror(errno));
        }
        
        // scan next input file name
        ret = fscanf(argument, "%s", file_name);
    }
        
    #ifdef __DEBUG
    // if debug, output the input file content.
    for (size_t i = 0; i < inputs_size; ++i) {
        // scan length of next line
        ret = fscanf(inputs[i], "%u", &current_len);
        while (ret > SUCCESS) {
            if (current_len > max_len) {
                // update max len
                max_len = current_len;
                // allocate a larger buffer
                if (line_buf) {
                    // initialized, call realloc.
                    line_buf = (char*)realloc(line_buf, max_len);
                }
                else {
                    // uninitialized, call malloc.
                    line_buf = (char*)malloc(max_len);
                }
            }
            // read in content including the end line character.
            ret = fscanf(inputs[i], " %s", line_buf);
            if (ret < SUCCESS) {
                perror("unexpected end of file.\n");
                break;
            }
            // print the contents, with a maximum of current_len characters.
            fprintf(output, "%.*s\n", current_len, line_buf);
            // scan length of next line
            ret = fscanf(inputs[i], "%u", &current_len);
        }
    }
    #endif

    // close files
    fcloseall();

    // free
    free(line_buf);
    free(inputs);

    return SUCCESS;
}
