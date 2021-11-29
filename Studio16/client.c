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
// for remote
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <netdb.h>

const char* SHELL_IP = "128.252.167.161";
const unsigned int PORT_NUM = 32768;

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
	// stores the internet domain socket address used.
	struct sockaddr_in name;
	// stores the return value of system calls.
	int ret;
	// stores a file pointer to the socket.
	FILE * data_fp;
	// stores the data and converted data to send.
	unsigned int data;
	
	// stores the host name and service name.
    char host_name[NI_MAXHOST], service_name[NI_MAXSERV];
	
	// create a socket for local connection.
	data_socket = socket(AF_INET, SOCK_STREAM, SOCKET_PROTOCAL);
	// on error, -1 is returned.
	if (data_socket < SUCCESS) return err_handler(ERR_SOCKET);
	printf("Created data socket %u.\n", data_socket);
	
	// connect to the communications channel.
	memset(&name, 0, sizeof(struct sockaddr_in));
	name.sin_family = AF_INET;
	name.sin_port = htons(PORT_NUM);
    inet_aton(SHELL_IP, &(name.sin_addr));
	ret = connect(data_socket, (const struct sockaddr *) &name,
                      sizeof(struct sockaddr_in));
	// on error, -1 is returned.
	if (ret < SUCCESS) return err_handler(ERR_CONNECT);
	printf("Connected data socket %u.\n", data_socket);
	
	// connected, get name info
	ret = getnameinfo((const struct sockaddr *) &name, sizeof(name), host_name, sizeof(host_name), service_name, sizeof(service_name), NI_NUMERICHOST|NI_NUMERICSERV);
	// on error, -1 is returned.
	if (ret < SUCCESS) return err_handler(ERR_HOST_NAME);
	printf("Connected to %s on port %s,", host_name, service_name);
	
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

