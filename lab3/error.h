// error.h
// declares error codes and error code handling protocol.

#ifndef ERROR_H
#define ERROR_H

// error codes
typedef enum error_code
{
    SUCCESS,
    INVALID_ARGUMENT,      // invalid number of arguments.
    FAILED_TO_OPEN_ARGUMENT,  // failed to open the file specified by the argument for read.
    FAILED_TO_OPEN_OUTPUT, // failed to open the first file for write.
    FAILED_TO_OPEN_INPUT,  // failed to open one of the subsequent files for read.
} error_code_t;

// print out the error.
// @param:
// - error_code: the error code of the error to print out.
// - message: any additional to print out, NULL if no additional message.
// @return: returns the error code.
unsigned int error_handler(unsigned int error_code, char *message);

#endif /* ERROR_H */