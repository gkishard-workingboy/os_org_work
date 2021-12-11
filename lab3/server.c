// server.c
// defines the server.

#include "server.h"
#include "error.h"

// uses GNU extension reallocarray() and fcloseall()
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
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
//for epoll
#include <sys/epoll.h>

// defines the maximum number of characters in a file name.
// 255 for linux.
#define NAME_MAX 255

/// read an input file and send it through a socket.
/// @param:
/// - in: the file pointer to the input.
/// - out: the file pointer to the socket.
void read_and_send(FILE* in, FILE* out)
{
    char* line_buf = NULL;
    int current_len = 0;
    unsigned int max_len = 0;
    
    // gets the next line from the input.
    current_len = getline(&line_buf, &max_len, in);
    while (current_len >= 0) {
        // writes it to the output.
        fprintf(out, "%.*s", current_len, line_buf);
        // gets the next line from the input.
        current_len = getline(&line_buf, &max_len, in);
    }
    
    free(line_buf);
}
    
/// main
int main(int argc, char* argv[])
{
	// stores the return value of system calls.
	int ret;

    // epoll file descriptor
    int epoll_fd;

    // store a file descriptor, usually one that currently will be used for I/O.
    int fd;
    
    // number of ready connections.
    int ready;

    // stores the file name.
    char file_name[NAME_MAX+1];
    
    // stores a file pointer to the initial input file specified by the argument.
    FILE *argument;
    
    // stores a file pointer to the output file specified by the initial input file.
    FILE *output;
    
    // stores an array of file pointers to the input files specified by the initial input file.
    // allocated dynamically on the heap at runtime.
    FILE **inputs;
    FILE **new;
    // inputs size.
    size_t inputs_size = 0;
    // max inputs capacity.
    size_t inputs_capacity = 4;
    
    /* remove this */
    // read from file
    unsigned int current_len = 0;
    unsigned int max_len = 0;
    char* line_buf = NULL;
    
    // stores the file descriptor for the sockets.
    unsigned int connection_socket, data_socket;
    // stores the internet domain socket address used.
    struct sockaddr_in name;
    // stores the internet domain socket address accepted.
    struct sockaddr_in addr;
    socklen_t addr_len;
    // stores a file pointer to the socket.
    FILE * data_fp;
    
    // stores the own host name.
    char server_host_name[NI_MAXHOST];
    // stores the own host name.
    unsigned int port_number;
    // stores the socket's host name and service name.
    char host_name[NI_MAXHOST], service_name[NI_MAXSERV];

    /* body */

    struct epoll_event event, evlist[EVENT_SIZE];

    // validate arguments
    if (argc != EXPECTED_ARGC)
    {
        // invalid number of arguments
        return error_handler(INVALID_ARGUMENT, argv[PROGRAM_NAME]);
    }

    // open initial file for read
    argument = fopen(argv[ARGUMENT_FILE_NAME], "r");
    if (argument == NULL)
    {
        // failed to open file.
        return error_handler(FAILED_TO_OPEN_ARGUMENT, strerror(errno));
    }

    // scan output file name
    ret = fscanf(argument, "%s", file_name);
    if (ret < SUCCESS)
    {
        // empty file.
        return error_handler(EMPTY_ARGUMENT_FILE, file_name);
    }
    // open output file for write
    output = fopen(file_name, "w");
    if (output == NULL)
    {
        // failed to open file.
        fcloseall();
        return error_handler(FAILED_TO_OPEN_OUTPUT, strerror(errno));
    }

    // allocate inputs
    inputs = calloc(inputs_capacity, sizeof(FILE*));
    if (inputs == NULL) {
        // calloc failed.
        fcloseall();
        return error_handler(OUT_OF_MEMORY, NULL);
    }
    // scan next input file name
    ret = fscanf(argument, "%s", file_name);
    while (ret >= SUCCESS) {
        // success, increment size.
        inputs_size = inputs_size + 1;
        // check if need to allocate new memory.
        if (inputs_size > inputs_capacity)
        {
            // reallocate inputs to be twice as big.
            inputs_capacity = inputs_capacity * 2; // optimize to be << 1.
            new = reallocarray(inputs, inputs_capacity, sizeof(FILE*));
            // free the old pointer.
            free(inputs);
            if (new == NULL)
            {
                // reallocarray failed.
                fcloseall();
                return error_handler(OUT_OF_MEMORY, NULL);
            }
            inputs = new;
        }
        // open input file for read
        inputs[inputs_size-1] = fopen(file_name, "r");
        if (inputs[inputs_size-1] == NULL)
        {
            // failed to open file.
            free(inputs);
            return error_handler(FAILED_TO_OPEN_INPUT, strerror(errno));
        }
        
        // scan next input file name
        ret = fscanf(argument, "%s", file_name);
    }
        
    #ifdef __DEBUG
    // if debug, output the input file content.
    for (size_t i = 0; i < inputs_size; ++i) {
        // scan length of next line
        ret = fscanf(inputs[i], "%u", &current_len);
        while (ret > SUCCESS) {
            if (current_len > max_len) {
                // update max len
                max_len = current_len;
                // allocate a larger buffer
                if (line_buf) {
                    // initialized, call realloc.
                    line_buf = (char*)realloc(line_buf, max_len);
                }
                else {
                    // uninitialized, call malloc.
                    line_buf = (char*)malloc(max_len);
                }
            }
            // read in content including the end line character.
            ret = fscanf(inputs[i], " %s", line_buf);
            if (ret < SUCCESS) {
                perror("unexpected end of file.\n");
                break;
            }
            // print the contents, with a maximum of current_len characters.
            fprintf(output, "%.*s\n", current_len, line_buf);
            // scan length of next line
            ret = fscanf(inputs[i], "%u", &current_len);
        }
    }
    #endif
    
    // convert port number to unsigned int.
    port_number = atoi(argv[PORT_NUMBER]);
    
    // create a socket for local connection.
    connection_socket = socket(AF_INET, SOCK_STREAM, SOCKET_PROTOCAL);
    // on error, -1 is returned.
    if (connection_socket < SUCCESS)
    {
        // close files.
        fcloseall();
        // free heap memory.
        return error_helper(ERR_SOCKET, strerror(errno), line_buf, inputs);
    }
    printf("Created connection socket %u.\n", connection_socket);
    
    // connected, get name info
    ret = gethostname(server_host_name, sizeof(server_host_name));
    // on error, -1 is returned.
    if (ret < SUCCESS)
    {
        fcloseall();
        return error_helper(ERR_LISTEN, strerror(errno), line_buf, inputs);
    }
    printf("Hosting on %s on port %u.\n", server_host_name, port_number);
    
    // create the communications channel.
    memset(&name, 0, sizeof(struct sockaddr_in));
    name.sin_family = AF_INET;
    name.sin_port = htons(port_number);
    name.sin_addr.s_addr = INADDR_ANY;
    ret = bind(connection_socket, (const struct sockaddr *) &name, sizeof(struct sockaddr_in));
    // on error, -1 is returned.
    if (ret < SUCCESS)
    {
        fcloseall();
        return error_helper(ERR_BIND, strerror(errno), line_buf, inputs);
    }
    
    // start listening on connection socket.
    ret = listen(connection_socket, SOCKET_BACKLOG_SIZE);
    // on error, -1 is returned.
    if (ret < SUCCESS)
    {
        fcloseall();
        return error_helper(ERR_LISTEN, strerror(errno), line_buf, inputs);
    }
	printf("Start listening on connection socket %u.\n", connection_socket);
    
    epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) {
        fcloseall();
        return error_helper(ERR_EP_CREATE, strerror(errno), line_buf, inputs);
    }

    ev.data.fd = connection_socket;
    ev.events = EPOLLIN;
    ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, &ev);
    if (ret < 0) {
        fcloseall();
        return error_helper(ERR_EP_CTL, strerror(errno), line_buf, inputs);
    }

    // while loop for accept socket connections.
    do {
        //epoll will block until a connection is ready for I/O
        if ((ready = epoll_wait(epoll_fd, evlist, EVENT_SIZE, -1)) > 0) {
            if (((ready)) < 0 && errno != EINTR) {
                perror("epoll_wait wrong.");
            }
            for (int i = 0; i < ready; i++) {
                evlist[i].data.fd;
                if (evlist[i].events & EPOLLIN) {
                    // input is ready
                    // new connection.
                    if (evlist[i].data.fd == connection_socket) {
                        if ((fd = accept(connection_socket, (struct sockaddr *)&addr, &addr_len)) < 0) {
                            fcloseall();
                            return error_helper(ERR_ACCEPT, strerror(errno), line_buf, inputs);
                        }
                        // register the new socket into epoll
                        ev.events = EPOLLIN | EPOLLRDHUP;
                        ev.data.fd = fd;
                        ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev);
                        if (ret < 0) {
                            fcloseall();
                            return error_helper(ERR_EP_CTL, strerror(errno), line_buf, inputs);
                        }
                    } else {
                        // peer disconnected.
                        fd = evlist[i].data.fd;
                        if (evlist[i].events & EPOLLRDHUP) {
                            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                            close(fd);
                        }
                        
                        // send current fragment to peer.
                        
                    }
                }
            }
        }
    } while (/*condition*/);

    // close the connection socket.
    printf("Closing connection socket %u.\n", connection_socket);
    close(connection_socket);
    
    // close files
    fcloseall();

    // free
    free(line_buf);
    free(inputs);

    return SUCCESS;
}
