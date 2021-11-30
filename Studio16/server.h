#ifndef SEVER_H
#define SEVER_H

#include <stdio.h>
// socket configs
#define SOCKET_PROTOCAL 0
#define SOCKET_NAME "/tmp/studio16.socket"
#define SOCKET_BACKLOG_SIZE 10
#define END_CODE 418
#define SHORT_CODE 200
#define ARRAY_CODE 201

/// error codes
enum err_code_t
{
	SUCCESS,
	ERR_HOST_NAME,
	ERR_SOCKET,
	ERR_BIND,
	ERR_LISTEN,
	ERR_ACCEPT,
	ERR_OPEN,
};

/// error description
static const char* err_desc[] =
{
	"Success",
	"Failed to get host name",
	"Failed to create connection socket",
	"Failed to bind socket",
	"Failed to listen to socket",
	"Failed to accept data socket",
	"Failed to open data socket",
};

/// handles errors by printing it to the standard error stream.
/// @param:
/// - err: the error code for the error to describe.
/// @return: err.
int err_handler(int err);

/// converts and reads one integer code from the client.
/// @param
/// - fp: a pointer to the file.
/// - int_data: the integer code to be read to.
/// @return 0 upon success, non-zero error code upon failure.
int recv_int(FILE* fp, int* int_data);

int recv_socket(FILE* fp);

/// main
int main(int argc, char* argv[]);

#endif /* SEVER_H */
