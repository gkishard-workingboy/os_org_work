// declarations
#include "client.h"
// utils
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

int send_int(FILE* fp, int int_data) {
	int ret;
	// data buffer
	unsigned int data;
	
	data = htonl(int_data);
	ret = fprintf(fp, "%u\n", data);
	if (ret < SUCCESS) return ERR_SEND;
	return SUCCESS;
}

int send_short(FILE* fp, int int_data) {
	int ret;
	
	ret = send_int(fp, SHORT_CODE);
	if (ret < SUCCESS) return ret;
	printf("Code sent: %u\n", SHORT_CODE);
	
	ret = send_int(fp, int_data);
	if (ret < SUCCESS) return ret;
	printf("Data sent: %u\n", int_data);
	
	if (ret < SUCCESS) return ERR_SEND;
	return SUCCESS;
}

int send_array(FILE* fp, int* array_data, int array_len) {
	int ret;
	
	ret = send_int(fp, ARRAY_CODE);
	if (ret < SUCCESS) return ret;
	printf("Code sent: %u\n", ARRAY_CODE);
	
	ret = send_int(fp, array_len);
	if (ret < SUCCESS) return ret;
	printf("Length sent: %u\n", array_len);
	
	for (int index = 0; index < array_len; ++index) {
		ret = send_int(fp, array_data[index]);
		if (ret < SUCCESS) return ret;
		printf("Data sent: %u\n", array_data[index]);
	}
	
	if (ret < SUCCESS) return ERR_SEND;
	return SUCCESS;
}

int send_end(FILE* fp) {	
	int ret;
	
	ret = send_int(fp, END_CODE);
	if (ret < SUCCESS) return ret;
	printf("Code sent: %u\n", END_CODE);
	return SUCCESS;
}

int main(int argc, char* argv[])
{
	int quit_flag, short_flag = 0;
	int int_array[DATA_MAX];
	// check argument counts
	if (argc > MAXIMUM_ARGC) {
		printf("usage: %s [quit|short]\n", argv[PROGRAM_NAME]);
		return ERR_ARG;
	}
	else if (argc == MAXIMUM_ARGC) {
		// check if quit
		if (strcmp(argv[ADDITIONAL_ARGUMENT], END_ARG) == 0) {
			quit_flag = 1;
		}
		// check if short
		else if (strcmp(argv[ADDITIONAL_ARGUMENT], SHORT_ARG) == 0) {
			short_flag = 1;
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
	unsigned int int_data;
	
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
	printf("Connected to %s on port %s.\n", host_name, service_name);
	
	// connected, open socket.
	data_fp = fdopen(data_socket, "w");
	// on error, NULL is returned.
	if (data_fp == NULL) return err_handler(ERR_OPEN);
	
	// send data.
	if (short_flag) {
		for (int_data = 0; int_data < DATA_MAX; ++int_data)
		{
			ret = send_short(data_fp, int_data);
			if (ret < SUCCESS) return err_handler(ret);
		}
	}
	else {
		for (int_data = 0; int_data < DATA_MAX; ++int_data)
		{
			int_array[int_data] = int_data;
		}
		ret = send_array(data_fp, int_array, DATA_MAX);
		if (ret < SUCCESS) return err_handler(ret);
	}
	
	// send end.
	if (quit_flag) {
		ret = send_end(data_fp);
		if (ret < SUCCESS) return err_handler(ret);
	}
	
	// close the file.
	fclose(data_fp);
	
	// close the data socket.
	printf("Closing data socket %u.\n", data_socket);
	close(data_socket);

	// successful execution
	return SUCCESS;
}

