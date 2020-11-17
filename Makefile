CPP = g++
CPPFLAGS = -Wall -g
SOURCES = src/client/client.cpp
OBJECTS = client.o

.PHONY: all clean client

all: client

client:
	g++ $(CPPFLAGS) $(SOURCES) -o client

clean:
	rm -rf src/client/client.o client 