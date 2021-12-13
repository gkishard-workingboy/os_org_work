# Lab 3

## Group Members
Zhikuan Wei <w.zhikuan@wustl.edu>
Zhuroan Sun <zhuoran.sun@wustl.edu>

## Server Design
The server first check the arguments. If the argument is malformed, it prints the usage message,
and returns an unique error code.

Then it opens the file specified by the argument in sequential order, and stores them into a 
dynamic length file pointer array. And the size of the file pointer array is also the size of 
connections we need. Then we create socket for listen incomming connection, and applying the 
epoll as the demultiplexer. and also use the min heap as the sorting data structure
(https://github.com/armon/c-minheap-array). First we register the listenning socket into epoll 
and epoll_wait function will give us the file descriptor ready for I/O operation. If the ready 
file descriptor is listenning socket, then we will accept the new connection and create a corresponding
rw_obj to it, also store such rw_obj in the rw_obj array, and store the corresponding index to con2rw_obj
array to make future access more efficient.

A normal connection will first be written by server, and we use the rw_obj to record how much data
has been written to prevent short-write happens. After all lines of that file being sent, we will 
fclose that file pointer and prepare the rw_obj for reading, and also change the event of connection
socket for input and disconnection. Then if socket is sending data back, our demultiplexer will 
detect such notification and server will read the data from that socket, in case of short-read, we 
store the raw data into rw_obj in a stream manner and search line from it, at last, we will compact 
the buffer inside the rw_obj for next reading. Each time we find a line from the buffer inside rw_obj,
we will easily separate line number and line content, then take number as the key content as the value 
to insert into min heap. 

After all clients finish their works, server will output all line content into an ouptut file in sorting order
by using the heap sort.



## Client Design
The client first check the arguments. If the argument is malformed, it prints the usage message,
and returns an unique error code.

Then it creates a data socket and try to connect to the server specified by the arguments.
If any step fails, it prints an error message describing the error, frees any dynamically 
allocated resource, and returns an unique error code.

After the client is connected to the server, it creates a min heap of <int, char *>
pairs. We used [this implementation of priority queue as a heap stored in an array]
(https://github.com/armon/c-minheap-array). The class pass an approriate comparasion 
function to the constructor such that it is a min heap. This allows us to insert a line 
and extract  the minimum line in O(log n) time, which allows our program to run in 
O(n log n) time. See the `HEAP_LICENSE` file for the license of the heap library.

The client wraps the socket in a file stream and uses higher level library functions for 
reading and writing data to handle short read and short writes. It then allocates an int to 
store the line number as key and a char * buffer for the line based on how much data is read 
before line deliminater. After reading a complete line, it inserts the <int, char *> pair into 
a min heap. When it reads EOF, the library function will return -1 and it stops reading.

Then it repeatedly extracts the minimum <int, char *> pair from the min heap and write it to 
the socket file stream then free the <int, char *> pair until it the heap is empty and no more 
pair could be extracted. The program has now been successfully executed and returns 0 to 
indicate success. We deal with short write by checking the return value from write and write
repeatedly until all content of a line is written.

## Build Instructions
Run `make` to build `server` and `client`.
Run `make server` to build `server` only.
Run `make client` to build `client` only.
Run `make clean` to clean.

## Testing and Evaluation
We built and tested our code modularly. And during our testing, we used `make server_shadow` and
`make client_shadow` to build program with symbols to use in gdb, which helped us locating the error.

We ran the test with both valid and invalid input and the program correctly prints the usage message
upon invalid argument, prints an error message if it couldn't open any of the files, and started listening
on the port specifiede on localhost if all inputs are valid.

When we tested our min heap implementation, we found that it prints uninitialized value to the output.
We realized that it requires the data we passed to the insert function to be a pointer to the dynamically 
allocated object. We dynamically allocate the key and it solved the problem. Now the client sends
the correct sorted data.

We ran the test for jabberwocky sample input, and at the beginning we noticed that we get malloc error such
as invalid next and double free error. After we fixed that, we noticed that the client is not
noticed when is the time to stop receive messages, so we design an STOP string with a key value -1 to 
denote the end of communication. Lastly, we find our writing from sever to client works well while the 
way back works badly. After long time finding the problem, we finally found out that the problem is 
because client fclose its FILE pointer early before the client writes the message back. In the end, we 
get the complete jabberwocky poem as output after we run the server once and the client five times.

Then we used a simple shell script to verify and time our program against jabberwocky.
```
pi@jordansunpi:~/Documents/CSE422S/lab3/operating_system_organization/lab3/test $ ./jabberwocky.sh 
Starting sever.
Created connection socket 10.
Hosting on jordansunpi on port 9090.
Start listening on connection socket 10.
16:02:48.510460626
Strating 5 clients
Created data socket 3.
Created data socket 3.
Connected data socket 3.
Connected to 0.0.0.0 on port 9090.
Connected data socket 3.
Connected to 0.0.0.0 on port 9090.
Created data socket 3.
Connected data socket 3.
Created data socket 3.
Connected to 0.0.0.0 on port 9090.
Connected data socket 3.
Connected to 0.0.0.0 on port 9090.
Created data socket 3.
Connected data socket 3.
Connected to 0.0.0.0 on port 9090.
Closing connection socket 10.
Server finishes task.
16:02:48.545113521
Showing differences (if any)
```
We saw that our program completed in about 35 miliseconds and
there's no difference between the original file and the output
file by comparing them using the diff command.

We then cut a larget text file and ran our program with the larget text file.
```
pi@jordansunpi:~/Documents/CSE422S/lab3/operating_system_organization/lab3/test $ ./github_terms_of_service.sh 
Starting sever.
Created connection socket 21.
Hosting on jordansunpi on port 9090.
Start listening on connection socket 21.
16:09:11.289004406
Strating 16 clients
Created data socket 3.
Created data socket 3.
Connected data socket 3.
Connected to 0.0.0.0 on port 9090.
Connected data socket 3.
Connected to 0.0.0.0 on port 9090.
Created data socket 3.
Connected data socket 3.
Connected to 0.0.0.0 on port 9090.
Created data socket 3.
Connected data socket 3.
Connected to 0.0.0.0 on port 9090.
Created data socket 3.
Connected data socket 3.
Connected to 0.0.0.0 on port 9090.
Created data socket 3.
Connected data socket 3.
Connected to 0.0.0.0 on port 9090.
Created data socket 3.
Created data socket 3.
Created data socket 3.
Connected data socket 3.
Connected to 0.0.0.0 on port 9090.
Created data socket 3.
Connected data socket 3.
Connected to 0.0.0.0 on port 9090.
Created data socket 3.
Connected data socket 3.
Connected to 0.0.0.0 on port 9090.
Created data socket 3.
Created data socket 3.
Connected data socket 3.
Connected to 0.0.0.0 on port 9090.
Connected data socket 3.
Connected to 0.0.0.0 on port 9090.
Created data socket 3.
Connected data socket 3.
Connected data socket 3.
Connected to 0.0.0.0 on port 9090.
Connected to 0.0.0.0 on port 9090.
Created data socket 3.
Connected data socket 3.
Connected to 0.0.0.0 on port 9090.
Connected data socket 3.
Connected to 0.0.0.0 on port 9090.
Created data socket 3.
Connected data socket 3.
Connected to 0.0.0.0 on port 9090.
Closing connection socket 21.
Server finishes task.
16:09:11.382107872
Showing differences (if any)
```
Our program ran successfully and took about 93 miliseconds.

## Development Efforts
Zhikuan Wei 16+ hours
Mainly worked on server socket and epoll.
Zhuroan Sun 19+ hours
Mainly worked on client socket and server file io.

