CPP = g++
CPPFLAGS = -Wall -g
SOURCES = server.cpp sockets.cpp chat.cpp rooms.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = server

.PHONY: all clean

all: $(EXECUTABLE)

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CPP) $(CPPFLAGS) -o $(EXECUTABLE) $(OBJECTS)

$(OBJECTS):
	$(CPP) -c $(CPPFLAGS) $(SOURCES)