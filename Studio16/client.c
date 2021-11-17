// declarations
#include "client.h"
// utils
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
// for socket(), bind()
#include <sys/types.h>
#include <sys/socket.h>
// for unix
#include <sys/un.h>

int err_handler(int err) {
	perror(err_desc[err]);
	return err;
}

int main(int argc, char* argv[])
{
	/// ref: man unix
	
	// stores the file descriptor for the sockets.
	unsigned int connection_socket, connection_socket;
	// stores the unix domain socket address.
	struct sockaddr_un name;
	// stores the return value of system calls.
	int ret;
	
	// unlink thesocket in case of previous exceptions.
	unlink(SOCKET_NAME);
	
	// create a socket for local connection.
	connection_socket = socket(AF_LOCAL, SOCK_STREAM, SOCKET_PROTOCAL);
	// on error, -1 is returned.
	if (connection_socket < SUCCESS) return err_handler(ERR_CON_SOCKET);
	
	// create the communications channel.
	memset(&name, 0, sizeof(struct sockaddr_un));
	name.sun_family = AF_LOCAL;
    strncpy(name.sun_path, SOCKET_NAME, sizeof(name.sun_path) - 1);
	ret = bind(connection_socket, (const struct sockaddr *) &name,
                      sizeof(struct sockaddr_un));
	// on error, -1 is returned.
	if (ret < SUCCESS) return err_handler(ERR_BIND);
	
	// start listening on connection socket.
	ret = listen(connection_socket, SOCKET_BACKLOG_SIZE);
	// on error, -1 is returned.
	if (ret < SUCCESS) return err_handler(ERR_LISTEN);
	
	// listening on connection socket
	data_socket = accept(connection_socket, NULL, NULL);
	// on error, -1 is returned.
	if (data_socket < SUCCESS) return err_handler(ERR_DATA_SOCKET);
	
	// close the data socket.
	close(data_socket);
    // close and unlink the connection socket.
    close(connection_socket);
    unlink(SOCKET_NAME);

	// successful execution
	return SUCCESS;
}

