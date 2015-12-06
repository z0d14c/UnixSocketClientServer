CC = g++
client = client.cpp
server = server.cpp

all:
	$(CC) $(client) -o client
	$(CC) $(server) -pthread -o server

clean:
	rm client server demo_socket