CC = g++
client = client.cpp
server = server.cpp

all:
	$(CC) $(client) -o client
	$(CC) $(server) -o server