CPP = g++
CPPFLAGS = -Wall -g -lncurses
SOURCES = main.cpp clui.cpp Client.cpp ../ClientBase.cpp WindowInterface.cpp
OBJECTS = main.o clui.o Client.o ClientBase.o WindowInterface.o
EXECUTABLE = client

.PHONY: all clean

all: $(EXECUTABLE)

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CPP) $(CPPFLAGS) -o $(EXECUTABLE) $(OBJECTS)

$(OBJECTS):
	$(CPP) -c $(CPPFLAGS) $(SOURCES)