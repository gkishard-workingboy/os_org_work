#ifndef SEVER_H
#define SEVER_H

// socket configs
#define SOCKET_PROTOCAL 0
#define SOCKET_NAME "/tmp/studio16.socket"
#define SOCKET_BACKLOG_SIZE 1

/// error codes
enum err_code_t
{
	SUCCESS,
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

/// main
int main(int argc, char* argv[]);

#endif /* SEVER_H */
