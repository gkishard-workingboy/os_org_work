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
	int quit = 0;
	// check argument counts
	if (argc > MAXIMUM_ARGC) {
		printf("usage: %s [quit]\n", argv[PROGRAM_NAME]);
		return ERR_ARG;
	}
	else if (argc == MAXIMUM_ARGC) {
		// check if quit
		if (strcmp(argv[ADDITIONAL_ARGUMENT], END_ARG) == 0) {
			quit = 1;
		}
	}
	
	/// ref: man unix
	
	// stores the file descriptor for the sockets.
	unsigned int data_socket;
	// stores the unix domain socket address.
	struct sockaddr_un name;
	// stores the return value of system calls.
	int ret;
	// stores a file pointer to the socket.
	FILE * data_fp;
	// stores the data and converted data to send.
	unsigned int data;
	
	// create a socket for local connection.
	data_socket = socket(AF_UNIX, SOCK_STREAM, SOCKET_PROTOCAL);
	// on error, -1 is returned.
	if (data_socket < SUCCESS) return err_handler(ERR_SOCKET);
	printf("Created data socket %u.\n", data_socket);
	
	// connect to the communications channel.
	memset(&name, 0, sizeof(struct sockaddr_un));
	name.sun_family = AF_UNIX;
    strncpy(name.sun_path, SOCKET_NAME, sizeof(name.sun_path) - 1);
	ret = connect(data_socket, (const struct sockaddr *) &name,
                      sizeof(struct sockaddr_un));
	// on error, -1 is returned.
	if (ret < SUCCESS) return err_handler(ERR_CONNECT);
	printf("Connected data socket %u.\n", data_socket);
	
	// connected, open socket.
	data_fp = fdopen(data_socket, "w");
	// on error, NULL is returned.
	if (data_fp == NULL) return err_handler(ERR_OPEN);
	
	// send data.
	for (data = 0; data < DATA_MAX; ++data)
	{
		ret = fprintf(data_fp, "%u\n", data);
		if (ret < SUCCESS) return err_handler(ERR_SEND);
		printf("Data sent: %u\n", data);
	}
	
	// send end.
	if (quit) {
		ret = fprintf(data_fp, "%u\n", END_CODE);
		if (ret < SUCCESS) return err_handler(ERR_SEND);
		printf("End sent: %u\n", END_CODE);
	}
	
	// close the file.
	fclose(data_fp);
	
	// close the data socket.
	printf("Closing data socket %u.\n", data_socket);
	close(data_socket);

	// successful execution
	return SUCCESS;
}

