// error.h
// declares error codes and error code handling protocol.

// error codes
typedef enum error_code
{
    SUCCESS,
    INVALID_ARGUMENT,      // invalid number of arguments.
    FAILED_TO_OPEN_INPUT,  // failed to open the file specified by the argument or one of the subsequent files for read.
    FAILED_TO_OPEN_OUTPUT, // failed to open the first file for write.
} error_code_t;

// print out the error.
// @param:
// - error_code: the error code of the error to print out.
// - message: any additional to print out, NULL if no additional message.
// @return: returns the error code.
int error_handler(int error_code, char* message);