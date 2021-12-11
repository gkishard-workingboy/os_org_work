targets = server client

all: $(targets)

server:	server.c error.c
	gcc $^ -o $@ -Wall

client:	client.c heap.c
	gcc $^ -o $@ -Wall

.phony:	clean

clean:
	rm $(targets) 
