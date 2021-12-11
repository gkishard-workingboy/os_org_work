// client.c
// defines the client.

#include "client.h"

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


// defines error code handling protocol.
// error messages
static const char* error_message[] =
{
    "Success",
    "Unexpected number of arguments",
    "Failed to get host name",
    "Failed to create data socket",
    "Failed to connect socket",
};

/// print out the error.
/// @param:
/// - error_code: the error code of the error to print out.
/// - message: any additional to print out, NULL if no additional message.
/// @return: returns the error code.
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

/// main
int main(int argc, char* argv[])
{
    /* variables */
    
    // stores the return value of system calls.
    int ret;

    // stores the file descriptor for the sockets.
    unsigned int data_socket;
    // stores the internet domain socket address used.
    struct sockaddr_in name;
    //// stores a file pointer to the socket.
    //FILE * data_fp;
    //// stores the data and converted data to send.
    //unsigned int int_data;

    // stores the own host name.
    unsigned int port_number;
    // stores the host name and service name.
    char host_name[NI_MAXHOST], service_name[NI_MAXSERV];

    /* body */

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
    
    return SUCCESS;
}
