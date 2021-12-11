targets = server client

all: $(targets)

server:	server.c
	gcc $^ -o $@ -Wall

client.:	client.c
	gcc $^ -o $@ -Wall

.phony:	clean

clean:
	rm $(targets) 
