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
// for remote
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <netdb.h>

const unsigned int PORT_NUM = 32768;

int err_handler(int err) {
	perror(err_desc[err]);
	return err;
}

int recv_int(FILE* fp, int* int_data) {
	int ret;
	int data = 0;
	ret = fscanf(fp, "%u", &data);
	if (ret < SUCCESS) return ret;
	*int_data = ntohl(data);
	return SUCCESS;
}

int recv_socket(FILE* fp) {
	int ret;
	int int_data = 0;
	int index, array_len;
	ret = recv_int(fp, &int_data);
	if (ret < SUCCESS) return ret;
	if (int_data == END_CODE) {
		printf("End received: %u\n", int_data);
		return END_CODE;
	} else if (int_data == SHORT_CODE) {
		ret = recv_int(fp, &int_data);
		if (ret < SUCCESS) return ret;
		// read successful, print out data.
		printf("Data received: %u\n", int_data);
	} else if (int_data == ARRAY_CODE) {
		ret = recv_int(fp, &array_len);
		if (ret < SUCCESS) return ret;
		// read successful, print out length.
		printf("Length received: %u\n", array_len);
		for (index = 0; index < array_len; ++index) {
			ret = recv_int(fp, &int_data);
			if (ret < SUCCESS) return ret;
			// read successful, print out data.
			printf("Data received: %u\n", int_data);
		}
	}
	return SUCCESS;
}

int main(int argc, char* argv[])
{
	/// ref: man unix
	
	// stores the file descriptor for the sockets.
	unsigned int connection_socket, data_socket;
	// stores the internet domain socket address used.
	struct sockaddr_in name;
	// stores the internet domain socket address accepted.
	struct sockaddr_in addr;
	socklen_t addr_len;
	// stores the return value of system calls.
	int ret;
	// stores a file pointer to the socket.
	FILE * data_fp;
	
	// stores the own host name.
	char server_host_name[NI_MAXHOST];
	// stores the socket's host name and service name.
	char host_name[NI_MAXHOST], service_name[NI_MAXSERV];
	
	// create a socket for local connection.
	connection_socket = socket(AF_INET, SOCK_STREAM, SOCKET_PROTOCAL);
	// on error, -1 is returned.
	if (connection_socket < SUCCESS) return err_handler(ERR_SOCKET);
	printf("Created connection socket %u.\n", connection_socket);
	
	// connected, get name info
	ret = gethostname(server_host_name, sizeof(server_host_name));
	// on error, -1 is returned.
	if (ret < SUCCESS) return err_handler(ERR_HOST_NAME);
	printf("Hosting on %s.\n", server_host_name);
	
	// create the communications channel.
	memset(&name, 0, sizeof(struct sockaddr_in));
	name.sin_family = AF_INET;
	name.sin_port = htons(PORT_NUM);
    name.sin_addr.s_addr = INADDR_ANY;
	ret = bind(connection_socket, (const struct sockaddr *) &name,
                      sizeof(struct sockaddr_in));
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
		addr_len = sizeof(addr);
		data_socket = accept(connection_socket,(struct sockaddr *) &addr, &addr_len);
		// on error, -1 is returned.
		if (data_socket < SUCCESS) return err_handler(ERR_ACCEPT);
		printf("Accepted data socket %u.\n", data_socket);
	
		// connected, get name info
		ret = getnameinfo((const struct sockaddr *) &addr, addr_len, host_name, sizeof(host_name), service_name, sizeof(service_name), NI_NUMERICHOST|NI_NUMERICSERV);
		// on error, -1 is returned.
		if (ret < SUCCESS) return err_handler(ERR_HOST_NAME);
		printf("Connected to %s on port %s.\n", host_name, service_name);
		
		// open socket.
		data_fp = fdopen(data_socket, "r");
		// on error, NULL is returned.
		if (data_fp == NULL) return err_handler(ERR_OPEN);
		
		// read data from the socket.
		ret = SUCCESS;
		while (ret >= SUCCESS)
		{
			ret = recv_socket(data_fp);
			if (ret == END_CODE) {
				end = 1;
			}
		}
		
		// read completed.
		fclose(data_fp);
	}
	
    // close and unlink the connection socket.
	printf("Closing connection socket %u.\n", connection_socket);
    close(connection_socket);

	// successful execution.
	return SUCCESS;
}

