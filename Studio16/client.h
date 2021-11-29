#ifndef CLIENT_H
#define CLIENT_H

// socket configs
#define SOCKET_PROTOCAL 0
#define SOCKET_NAME "/tmp/studio16.socket"
#define DATA_MAX 100
#define END_ARG "quit"
#define END_CODE 418

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
/// @param:
/// - err: the error code for the error to describe.
/// @return: err.
int err_handler(int err);

/// main
int main(int argc, char* argv[]);

#endif /* CLIENT_H */
