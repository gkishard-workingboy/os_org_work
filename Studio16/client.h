#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
// socket configs
#define SOCKET_PROTOCAL 0
#define SOCKET_NAME "/tmp/studio16.socket"
#define DATA_MAX 100
#define END_ARG "quit"
#define END_CODE 418
#define SHORT_ARG "short"
#define SHORT_CODE 200
#define ARRAY_CODE 201

/// arguments
enum arguments_t {
	PROGRAM_NAME,
	ADDITIONAL_ARGUMENT,
	MAXIMUM_ARGC,
};

/// error codes
enum err_code_t {
	SUCCESS,
	ERR_ARG,
	ERR_HOST_NAME,
	ERR_SOCKET,
	ERR_CONNECT,
	ERR_SEND,
	ERR_OPEN,
};

/// error description
static const char* err_desc[] =
{
	"Success",
	"Unexpected number of arguments",
	"Failed to get host name",
	"Failed to create data socket",
	"Failed to connect socket",
	"Failed to write to socket",
	"Failed to open data socket",
};

/// handles errors by printing it to the standard error stream.
/// @param
/// - err: the error code for the error to describe.
/// @return err.
int err_handler(int err);

/// converts and sends one integer code to the server.
/// @param
/// - fp: a pointer to the file.
/// - int_data: the integer code to send.
/// @return 0 upon success, non-zero error code upon failure.
int send_int(FILE* fp, int int_data);

/// converts and sends a short message with only one integer to the server.
/// @param
/// - fp: a pointer to the file.
/// - int_data: the single integer to send.
/// @return 0 upon success, non-zero error code upon failure.
int send_short(FILE* fp, int int_data);

/// converts and sends an array message with an array of integer to the server.
/// @param
/// - fp: a pointer to the file.
/// - array_data: the array of integer to send.
/// - array_len: the length of the array to send.
/// @return 0 upon success, non-zero error code upon failure.
int send_array(FILE* fp, int* array_data, int array_len);


/// converts and sends the end code to the server.
/// @param
/// - fp: a pointer to the file.
/// @return 0 upon success, non-zero error code upon failure.
int send_end(FILE* fp);

/// main
int main(int argc, char* argv[]);

#endif /* CLIENT_H */
