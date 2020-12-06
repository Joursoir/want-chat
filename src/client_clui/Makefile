CPP = g++
CPPFLAGS = -Wall -g -lncurses
SOURCES = client.cpp clui.cpp user.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = client

.PHONY: all clean

all: $(EXECUTABLE)

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CPP) $(CPPFLAGS) -o $(EXECUTABLE) $(OBJECTS)

$(OBJECTS):
	$(CPP) -c $(CPPFLAGS) $(SOURCES)