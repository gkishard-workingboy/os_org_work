// client.c
// defines the client.

#include "client.h"

#include <stdlib.h>
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

// defines error code handling protocol.
// error messages
static const char* error_message[] =
{
    "Success",
    "Unexpected number of arguments",
    "Failed to get host name",
    "Failed to create data socket",
    "Failed to connect socket",
    "Failed to open socket for read",
    "Failed to open socket for write",
};

/// print out the error.
/// @param error_code: the error code of the error to print out.
/// @param message: any additional to print out, NULL if no additional message.
/// @return returns the error code.
unsigned int error_handler(unsigned int error_code, char *message)
{
    if (error_code == INVALID_ARGUMENT)
    {
        // invalid argument.
        fprintf(stderr, "%s\n", error_message[error_code]);
        // print usage message.
        if (message == NULL)
        {
            // no additional message, program fault.
            fprintf(stderr, "Usage: [program] [hostname] [port]\n");
        }
        else
        {
            // additional message.
            fprintf(stderr, "Usage: %s [hostname] [port]\n", message);
        }
    }
    else {
        // program error, print error message.
        if (message == NULL) {
            // no additional message.
            fprintf(stderr, "%s\n", error_message[error_code]);
        } else
        {
            // additional message.
            fprintf(stderr, "%s: %s\n", error_message[error_code], message);
        }
    }
    return error_code;
}

/// compares two keys
int cmp_node(void* lhs, void* rhs)
{
    int l = *(int *)lhs;
    int r = *(int *)rhs;
    if (l == r) return 0;
    if (l > r) return 1;
    return -1;
}

/// read one pair of unsigned int and string and insert into the min heap.
/// @param stream: the file pointer to the data socket.
/// @param root: the root of the min heap.
/// @return -1 upon failure, 0 upon success.
int read_and_insert(FILE* stream, heap* root)
{
    // return value
    int ret = 0;
    // line number, used as key for the min heap.
    int* key = (int*)malloc(sizeof(int));
    // line buffer and len that holds the line
    // used as value for the min heap.
    char* line_buf = NULL;
    unsigned int len = 0;
    
    // scan length of next line
    ret = fscanf(stream, "%d", key);
    if (ret < 0) {
        // read failed.
        return -1;
    }
    
    // discard the space delim
    ret = fgetc(stream);
    if (ret < 0) {
        // read failed.
        return -1;
    }
    
    // get one line from the input.
    ret = getline(&line_buf, &len, stream);
    if (ret < 0) {
        // read failed.
        return -1;
    }
    
    // insert
    heap_insert(root, key, line_buf);
    
    return len;
}

/// main
int main(int argc, char* argv[])
{
    // stores the return value of system calls.
    int ret;
    int current_len = 0;
    int max_len = 0;
    
    /* sockets */
    
    // stores the file descriptor for the sockets.
    unsigned int data_socket;
    // stores the internet domain socket address used.
    struct sockaddr_in name;
    // stores a file pointer to the socket.
    FILE * data_fp;

    // stores the own host name.
    unsigned int port_number;
    // stores the host name and service name.
    char host_name[NI_MAXHOST], service_name[NI_MAXSERV];

    // validate arguments
    if (argc != EXPECTED_ARGC)
    {
        // invalid number of arguments
        return error_handler(INVALID_ARGUMENT, argv[PROGRAM_NAME]);
    }
    // convert port number to unsigned int.
    port_number = atoi(argv[PORT_NUMBER]);
    
    // create a socket for local connection.
    data_socket = socket(AF_INET, SOCK_STREAM, SOCKET_PROTOCAL);
    // on error, -1 is returned.
    if (data_socket < SUCCESS)
    {
        return error_handler(ERR_SOCKET, strerror(errno));
    }
    printf("Created data socket %u.\n", data_socket);
    
    // connect to the communications channel.
    memset(&name, 0, sizeof(struct sockaddr_in));
    name.sin_family = AF_INET;
    name.sin_port = htons(port_number);
    inet_aton(argv[HOST_NAME], &(name.sin_addr));
    ret = connect(data_socket, (const struct sockaddr *) &name,
                      sizeof(struct sockaddr_in));
    // on error, -1 is returned.
    if (ret < SUCCESS)
    {
         return error_handler(ERR_CONNECT, strerror(errno));
     }
    printf("Connected data socket %u.\n", data_socket);
    
    // connected, get name info
    ret = getnameinfo((const struct sockaddr *) &name, sizeof(name), host_name, sizeof(host_name), service_name, sizeof(service_name), NI_NUMERICHOST|NI_NUMERICSERV);
    // on error, -1 is returned.
    if (ret < SUCCESS)
    {
        return error_handler(ERR_HOST_NAME, strerror(errno));
    }
    printf("Connected to %s on port %s.\n", host_name, service_name);
    
	// connected, open socket for read.
	data_fp = fdopen(data_socket, "r+");
	// on error, NULL is returned.
	if (data_fp == NULL)
    {
        return error_handler(ERR_READ, strerror(errno));
    }
    
    // create a min heap
    heap root;
    heap_create(&root, 0, cmp_node);
    
    // read from file until fail.
    current_len = read_and_insert(data_fp, &root);
    while (current_len >= 0) {
        if (current_len > max_len) {
            max_len = current_len;
        }
        current_len = read_and_insert(data_fp, &root);
    }
    
    // write to socket.
    int* key;
    char* value;
    ret = heap_delmin(&root, (void**)&key, (void**)&value);
    while (ret) {
        // the end of the string contains endline character.
        fprintf(data_fp, "%d %s", *key, value);
        // free the key, value pair to avoid dangling pointers.
        free(key);
        free(value);
        ret = heap_delmin(&root, (void**)&key, (void**)&value);
    }
    
    // free the min heap
    heap_destroy(&root);
    // close socket for write.
    fclose(data_fp);
    return SUCCESS;
}
