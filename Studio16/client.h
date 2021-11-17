#ifndef CLIENT_H
#define CLIENT_H

// socket configs
#define SOCKET_PROTOCAL 0
#define SOCKET_NAME "/tmp/studio16.socket"
#define DATA_MAX 100

/// error codes
enum err_code_t
{
	SUCCESS,
	ERR_SOCKET,
	ERR_CONNECT,
	ERR_SEND,
	ERR_OPEN,
};

/// error description
static const char* err_desc[] =
{
	"Success",
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