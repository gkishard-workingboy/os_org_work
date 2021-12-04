// declarations
#include "server.h"
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
	unsigned int connection_socket, data_socket;
	// stores the unix domain socket address.
	struct sockaddr_un name;
	// stores the return value of system calls.
	int ret;
	// stores a file pointer to the socket.
	FILE * data_fp;
	// stores the data read.
	unsigned int data;
	
	// unlink the socket in case of previous exceptions.
	unlink(SOCKET_NAME);
	
	// create a socket for local connection.
	connection_socket = socket(AF_UNIX, SOCK_STREAM, SOCKET_PROTOCAL);
	// on error, -1 is returned.
	if (connection_socket < SUCCESS) return err_handler(ERR_SOCKET);
	printf("Created connection socket %u.\n", connection_socket);
	
	// create the communications channel.
	memset(&name, 0, sizeof(struct sockaddr_un));
	name.sun_family = AF_UNIX;
    strncpy(name.sun_path, SOCKET_NAME, sizeof(name.sun_path) - 1);
	ret = bind(connection_socket, (const struct sockaddr *) &name,
                      sizeof(struct sockaddr_un));
	// on error, -1 is returned.
	if (ret < SUCCESS) return err_handler(ERR_BIND);
	
	// start listening on connection socket.
	ret = listen(connection_socket, SOCKET_BACKLOG_SIZE);
	// on error, -1 is returned.
	if (ret < SUCCESS) return err_handler(ERR_LISTEN);
	printf("Start listening on connection socket %u.\n", connection_socket);
	
	// continue accepting data sockets until end is sent.
	int end = 0;
	while (!end) {
		// accept data socket.
		data_socket = accept(connection_socket, NULL, NULL);
		// on error, -1 is returned.
		if (data_socket < SUCCESS) return err_handler(ERR_ACCEPT);
		printf("Accepted data socket %u.\n", data_socket);
		
		// connected, open socket.
		data_fp = fdopen(data_socket, "r");
		// on error, NULL is returned.
		if (data_fp == NULL) return err_handler(ERR_OPEN);
		
		// read data from the socket.
		ret = fscanf(data_fp, "%u", &data);
		while (ret >= SUCCESS)
		{
			if (data == END_CODE) {
				// read successful, print out data.
				printf("End received: %u\n", data);
				// ending.
				end = 1;
				break;
			}
			else {
				// read successful, print out data.
				printf("Data received: %u\n", data);
				// read data from the socket.
				ret = fscanf(data_fp, "%u", &data);
			}
		}
		
		// read completed.
		fclose(data_fp);
	}
	
    // close and unlink the connection socket.
	printf("Closing connection socket %u.\n", connection_socket);
    close(connection_socket);
    unlink(SOCKET_NAME);

	// successful execution.
	return SUCCESS;
}

