// server.c
// defines the server.

#include "server.h"
#include "error.h"
#include "heap.h"

// defines the maximum number of characters in a file name.
// 255 for linux.
#define NAME_MAX 255

/// read a line from input file and send it through a socket file descriptor.
/// @param in: the rw_obj pointer to the input.
/// @param out: the file pointer to the socket.
/// @return normally returns the number of characters read, or -1 if the file is read empty.
int read_and_send(rw_obj *in, int out)
{
    // last time is a short write.    
    if (in->last_len < in->line_len) {
        in->last_len += write(out, in->line_buf + in->last_len, in->line_len - in->last_len);
    } else {
        // gets the next line from the input.
        in->line_len = getline(&in->line_buf, &in->line_len, in->fptr);
        if (in->line_len == -1) {
            write(out, STOP, strlen(STOP));
        } else {
            // write a line to the output.
            in->last_len = write(out, in->line_buf, in->line_len);
        }
    }
    return in->line_len;
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

/// insert string into the min heap.
/// @param line: the input string.
/// @param len: lenght of string.
/// @param root: the specified heap.
/// @return -1 upon stop.
int insert_string(char* line, unsigned int len, heap* root)
{
    // index value
    int i = 0;
    // line number, used as key for the min heap.
    int* key = (int*)malloc(sizeof(int));
    // line buffer and len that holds the line
    // used as value for the min heap.
    char* line_buf = NULL;

    for (i = 0; i < len; ++i) {
        if (line[i] == ' ') {
            // seperate line number and line contents.
            line[i] = '\0';
            *key = atoi(line);
            // copy line content into a new string.
            line_buf = calloc(len - i, sizeof(char));
            strncpy(line_buf, line + i + 1, len - i - 1);
            // add null terminator to the end of string.
            line_buf[len - i - 1] = '\0';
            break;
        }
    }
    if (*key >= 0) {
        // insert such line into heap.
        heap_insert(root, key, line_buf);
    }

    return *key;
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

    // array converting connection to rw_obj.
    int *con2rw_obj; 

    // stores the file name.
    char file_name[NAME_MAX+1];
    
    // stores a file pointer to the initial input file specified by the argument.
    FILE *argument;
    
    // stores a file pointer to the output file specified by the initial input file.
    FILE *output;
    
    // stores an array of file pointers to the input files specified by the initial input file.
    // allocated dynamically on the heap at runtime.
    FILE **inputs;

    // inputs size.
    size_t inputs_size = 0;
    // max inputs capacity.
    size_t inputs_capacity = 4;
    // connections size
    size_t connections_size = 0;
    // connections limit
    size_t connections_limit;
    
    // stores the file descriptor for the sockets.
    unsigned int connection_socket;
    // stores the internet domain socket address used.
    struct sockaddr_in name;
    // stores the internet domain socket address accepted.
    struct sockaddr_in addr;
    socklen_t addr_len;
    
    // stores the line data read from the socket.
    char rd_buf[RD_BUF_SIZE];
    // stores the own host name.
    char server_host_name[NI_MAXHOST];
    // stores the own host name.
    unsigned int port_number;

    // store an array of rw_obj containing corresponding rw_obj for each socket.
    rw_obj *rw_objs;
    // point to specified rw_obj for further processing.
    rw_obj *data_rw_obj;

    // a min heap, initialized later.
    heap heap_root;
    // heap variable bundle.
    int* key;
    char* value;

    struct epoll_event ev, evlist[EVENT_SIZE];


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
            inputs = reallocarray(inputs, inputs_capacity, sizeof(FILE*));
            if (inputs == NULL)
            {
                // reallocarray failed.
                fcloseall();
                return error_handler(OUT_OF_MEMORY, NULL);
            }
        }
        // open input file for read
        inputs[inputs_size-1] = fopen(file_name, "r");
        if (inputs[inputs_size-1] == NULL)
        {
            fcloseall();
            // failed to open file.
            free(inputs);
            return error_handler(FAILED_TO_OPEN_INPUT, strerror(errno));
        }
        
        // scan next input file name
        ret = fscanf(argument, "%s", file_name);
    }

    rw_objs = calloc(inputs_size, sizeof(rw_obj));
    if (rw_objs == NULL) {
        fcloseall();
        return error_helper(OUT_OF_MEMORY, NULL, inputs, NULL);
    }

    // the limit of connections is input size
    connections_limit = inputs_size;

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

    // allocate memory for con2rw_obj array
    con2rw_obj = calloc(2*inputs_size + connection_socket, sizeof(int));
    if (con2rw_obj == NULL) {
        fcloseall();
        return error_helper(OUT_OF_MEMORY, NULL, inputs, NULL);
    }
    
    epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) {
        fcloseall();
        return error_helper(ERR_EP_CREATE, strerror(errno), rw_objs, inputs);
    }

    ev.data.fd = connection_socket;
    ev.events = EPOLLIN;
    ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connection_socket, &ev);
    if (ret < 0) {
        fcloseall();
        return error_helper(ERR_EP_CTL_ADD, strerror(errno), rw_objs, inputs);
    }

    // initialize heap.
    heap_create(&heap_root, 0, cmp_node);

    // while loop for accept socket connections.
    do {
        //epoll will block until a connection is ready for I/O
        if ((ready = epoll_wait(epoll_fd, evlist, EVENT_SIZE, -1)) > 0) {
            if (ready < 0 && errno != EINTR) {
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
                        ev.events = EPOLLOUT;
                        ev.data.fd = fd;
                        ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev);
                        if (ret < 0) {
                            fcloseall();
                            return error_helper(ERR_EP_CTL_ADD, strerror(errno), rw_objs, inputs);
                        }
                        con2rw_obj[fd] = connections_size;
                        // assign a rw_obj for new session.
                        data_rw_obj = &rw_objs[connections_size];
                        // initialize it
                        data_rw_obj->fptr = inputs[connections_size];
                        data_rw_obj->last_len = data_rw_obj->line_len = 0;
                        data_rw_obj->line_buf = NULL;
                        
                        printf("socket %d connecting.\n", fd);

                        if (++connections_size == inputs_size) {
                            ret = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, connection_socket, NULL);
                            if (ret < SUCCESS) {
                                fcloseall();
                                return error_helper(ERR_EP_CTL_DEL, strerror(errno), rw_objs, inputs);
                            }
                            close(connection_socket);
                            // close the connection socket.
                            printf("Closing connection socket %u.\n", connection_socket);
                        }
                    // data from client.
                    }else {
                        fd = evlist[i].data.fd;
                        
                        data_rw_obj = &rw_objs[con2rw_obj[fd]];

                        ret = read(fd, data_rw_obj->line_buf + data_rw_obj->last_len, RD_BUF_SIZE - data_rw_obj->last_len);
                        
                        // read data from client. (At most RD_BUF_SIZE)
                        data_rw_obj->last_len += ret;

                        // search for a line, and do insert for each line.
                        data_rw_obj->line_len = 0;
                        for (int j = 0; j < data_rw_obj->last_len; ++j) {
                            if (data_rw_obj->line_buf[j] == '\n'){
                                strncpy(rd_buf, data_rw_obj->line_buf + data_rw_obj->line_len, j - data_rw_obj->line_len + 1);
                                printf("%.*s\n", j-data_rw_obj->line_len+1, rd_buf);
                                // insert line into heap.
                                insert_string(rd_buf, j - data_rw_obj->line_len + 1, &heap_root);
                                // lets start searching for more line.
                                data_rw_obj->line_len = j + 1;
                            }
                        }
                        // compact line_buf
                        if (data_rw_obj->line_len > 0) {
                            data_rw_obj->last_len -= data_rw_obj->line_len;
                            strncpy(data_rw_obj->line_buf, data_rw_obj->line_buf + data_rw_obj->line_len, data_rw_obj->last_len);
                        }
                        printf("socket %d ret %d \n", fd, ret);
                        
                        if (ret == 0){
                            ev.events = EPOLLRDHUP;
                            ev.data.fd = fd;
                            ret = epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev);
                        }
                    }
                // output is ready
                } else if (evlist[i].events & EPOLLOUT) {
                        fd = evlist[i].data.fd;

                        // store the corresponding file pointer of current connection socket.
                        data_rw_obj = &rw_objs[con2rw_obj[fd]];

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
                    fd = evlist[i].data.fd;
                    data_rw_obj = &rw_objs[con2rw_obj[fd]];
                    free(data_rw_obj->line_buf);
                    printf("socket %d closed.\n", fd);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);
                    
                    // all tasks of client have been finished. 
                    --connections_limit;
                }
            }
        }
    } while (connections_limit > 0);

    // keep pop the top object of heap, and output it to the output file until no object left in heap.
    ret = heap_delmin(&heap_root, (void**)&key, (void**)&value);
    while (ret) {
        fprintf(output, "%s", value);
        // free the dynamically allocated object!
        free(key);
        free(value);
        ret = heap_delmin(&heap_root, (void**)&key, (void**)&value);
    }

    // free the min heap
    heap_destroy(&heap_root);

    // print end point
    printf("server finishes task.");

    // close files
    fcloseall();

    // free
    free(rw_objs);
    free(con2rw_obj);
    free(inputs);

    return SUCCESS;
}
