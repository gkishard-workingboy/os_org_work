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

/// read a line from input file and send it through a socket file descriptor.
/// @param:
/// - in: the file pointer to the input.
/// - out: the file pointer to the socket.
int read_and_send(rw_obj * in, int out)
{
    // last time is a short write.    
    if (in->last_len < in->line_len) {
        in->last_len += write(out, in->line_buf + in->last_len, in->line_len - in->last_len);
    } else {
        free(in->line_buf);
        // gets the next line from the input.
        in->line_len = in->last_lengetline(&in->line_buf, &in->line_len, in->fptr);
        if (current_len == -1) {
            fclose(in->fptr);
        } else {
            // write a line to the output.
            in->last_len = write(out, in->line_buf, current_len);
        }
    }
    return current_len;
}

typedef struct read_write_object {
    unsigned int last_len;
    unsigned int line_len;
    char* line_buf;
    FILE* fptr;
} rw_obj;
    
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

    // stores a file pointer as the corresponding source for specified connection. 
    File **data_output;

    // inputs size.
    size_t inputs_size = 0;
    // max inputs capacity.
    size_t inputs_capacity = 4;
    // connections size
    size_t connections_size = 0;
    
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
    
    // stores the line data read from the socket.
    char rd_buf[RD_BUF_SIZE];
    // stores the own host name.
    char server_host_name[NI_MAXHOST];
    // stores the own host name.
    unsigned int port_number;
    // stores the socket's host name and service name.
    char host_name[NI_MAXHOST];

    // store an array of rw_obj containing corresponding rw_obj for each socket.
    rw_obj *rw_objs;
    // point to specified rw_obj for further processing.
    rw_obj *data_rw_obj;

    struct epoll_event event, evlist[EVENT_SIZE];


    /* body */
    
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

    rw_objs = calloc(inputs_size, sizeof(rw_obj));
    if (rw_objs == NULL) {
        return error_helper(OUT_OF_MEMORY, NULL, inputs, NULL);
    }

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
        return error_helper(ERR_SOCKET, strerror(errno), rw_objs, inputs);
    }
    printf("Created connection socket %u.\n", connection_socket);
    
    // connected, get name info
    ret = gethostname(server_host_name, sizeof(server_host_name));
    // on error, -1 is returned.
    if (ret < SUCCESS)
    {
        fcloseall();
        return error_helper(ERR_LISTEN, strerror(errno), rw_objs, inputs);
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
        return error_helper(ERR_BIND, strerror(errno), rw_objs, inputs);
    }
    
    // start listening on connection socket.
    ret = listen(connection_socket, SOCKET_BACKLOG_SIZE);
    // on error, -1 is returned.
    if (ret < SUCCESS)
    {
        fcloseall();
        return error_helper(ERR_LISTEN, strerror(errno), rw_objs, inputs);
    }
	printf("Start listening on connection socket %u.\n", connection_socket);
    
    epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) {
        fcloseall();
        return error_helper(ERR_EP_CREATE, strerror(errno), rw_objs, inputs);
    }

    ev.data.fd = connection_socket;
    ev.events = EPOLLIN;
    ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, &ev);
    if (ret < 0) {
        fcloseall();
        return error_helper(ERR_EP_CTL_ADD, strerror(errno), rw_objs, inputs);
    }

    // while loop for accept socket connections.
    do {
        //epoll will block until a connection is ready for I/O
        if ((ready = epoll_wait(epoll_fd, evlist, EVENT_SIZE, -1)) > 0) {
            if (((ready)) < 0 && errno != EINTR) {
                perror("epoll_wait wrong.");
            }
            for (int i = 0; i < ready; i++) {
                // input is ready
                if (evlist[i].events & EPOLLIN) {
                    // new connection.
                    if (evlist[i].data.fd == connection_socket) {    
                        if ((fd = accept(connection_socket, (struct sockaddr *)&addr, &addr_len)) < 0) {
                            fcloseall();
                            return error_helper(ERR_ACCEPT, strerror(errno), rw_objs, inputs);
                        }
                        // register the new socket into epoll
                        ev.events = EPOLLOUT | EPOLLRDHUP;
                        ev.data.fd = fd;
                        ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev);
                        if (ret < 0) {
                            fcloseall();
                            return error_helper(ERR_EP_CTL_ADD, strerror(errno), rw_objs, inputs);
                        }
                        // assign a rw_obj for new session.
                        data_rw_obj = rw_objs[connections_size];
                        // initialize it
                        data_rw_obj->fptr = inputs[connections_size];
                        data_rw_obj->last_len = data_rw_obj->line_len = 0;
                        data_rw_obj->line_buf = NULL;

                        if (++connections_size == inputs_size) {
                            ret = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, connection_socket, NULL);
                            if (ret < SUCCESS) {
                                fcloseall();
                                return error_helper(ERR_EP_CTL_DEL, strerror(errno), rw_objs, inputs);
                            }
                            close(connections_socket);
                            // close the connection socket.
                            printf("Closing connection socket %u.\n", connection_socket);
                        }
                    // data from client.
                    }else {
                        fd = evlist[i].data.fd;
                        
                        data_rw_obj = inputs[fd - connections_size - 1];

                        // read data from client. (At most RD_BUF_SIZE)
                        data_rw_obj->last_len += read(fd, data_rw_obj->line_buf + data_rw_obj->last_len, RD_BUF_SIZE - data_rw_obj->last_len);
                        // search for a line, and do insert for each line.
                        for (int i = 0, data_rw_obj->line_len = 0; i < data_rw_obj->last_len; ++i) {
                            if (data_rw_obj->line_buf[i] == '\n'){
                                strncpy(rd_buf, data_rw_obj->line_buf + data_rw_obj->line_len, i - data_rw_obj->line_len + 1);
                                data_rw_obj->line_len = i + 1;
                            }
                            //TODO insert
                            
                        }
                        // compact line_buf
                        if (data_rw_obj->line_len > 0) {
                            strncpy(data_rw_obj->line_buf, data_rw_obj->line_buf + data_rw_obj->line_len, data_rw_obj->last_len - data_rw_obj->line_len);
                        }
                    }
                // output is ready
                } else if (evlist[i].events & EPOLLOUT) {
                        fd = evlist[i].data.fd;

                        // store the corresponding file pointer of current connection socket.
                        data_rw_obj = inputs[fd - connections_size - 1];

                        // client read from corresponding file pointer. 
                        ret = read_and_send(data_rw_obj, fd);

                        // the corresponding file is completely read.
                        if (ret == -1) {
                            // modify corresponding rw_obj to adjust for following reading.
                            fclose(data_rw_obj->fptr);
                            data_rw_obj->last_len = data_rw_obj->line_len = 0;
                            data_rw_obj->line_buf = calloc(RD_BUF_SIZE, sizeof(char));

                            // modify current socket so that it don't need to be written again.
                            ev.events = EPOLLIN | EPOLLRDHUP;
                            ev.data.fd = fd;
                            epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev);
                        }
                // peer disconnected.
                } else if (evlist[i].events & EPOLLRDHUP) {
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);
                }
            }
        }
    } while (/*condition*/);

    // print end point
    printf("server finishes task.");

    // close files
    fcloseall();

    // free
    free(rw_objs);
    free(inputs);

    return SUCCESS;
}
