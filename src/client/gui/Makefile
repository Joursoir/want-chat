CPP = g++
CPPFLAGS = -Wall -g -lfltk
SOURCES = main.cpp OO_FLTK.cpp ../ClientBase.cpp Client.cpp
OBJECTS = main.o OO_FLTK.o ClientBase.o Client.o
EXECUTABLE = client

.PHONY: all clean

all: $(EXECUTABLE)

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CPP) $(CPPFLAGS) -o $(EXECUTABLE) $(OBJECTS)

$(OBJECTS):
	$(CPP) -c $(CPPFLAGS) $(SOURCES)