# Lab 3

## Group Members
Zhikuan Wei
Zhuroan Sun

## Server Design

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

Then it repeatedly extracts the minimum <int, char *> pair from the min heap and prints it to 
the socket file stream until it the heap is empty and no more pair could be extracted. The 
program has been successfully executed and returns 0 to indicate success.

## Build Instructions
Run `make` to build `server` and `client`.
Run `make clean` to clean.

## Testing and Evaluation
We built and tested our code modularly. When we tested our min heap implementation,
we found that it prints uninitialized value to the output. We realized that it requires
the data we passed to the insert function to be a pointer to the dynamically allocated object.

## Efforts
Zhikuan Wei
Zhuroan Sun

