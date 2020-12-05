CPP = g++
CPPFLAGS = -Wall -g $(shell mariadb_config --cflags --libs)
SOURCES = main.cpp sockets.cpp ChatServer.cpp UserInfo.cpp UserInfo_io.cpp ChatRoom.cpp ChatRoom_cmd.cpp database.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = server

.PHONY: all clean

all: $(EXECUTABLE)

clean:
	@rm -rf $(OBJECTS) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	@$(CPP) $(CPPFLAGS) -o $(EXECUTABLE) $(OBJECTS)

$(OBJECTS):
	@$(CPP) -c $(CPPFLAGS) $(SOURCES)