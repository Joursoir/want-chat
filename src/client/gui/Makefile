CPP = g++
CPPFLAGS = -Wall -g -lfltk
SOURCES = client.cpp OO_FLTK.cpp
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