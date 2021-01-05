CPP = g++
CPPFLAGS = -Wall -g
LDFLAGS = 
ifeq ($(OS),Windows_NT)
	CPPFLAGS += -static-libgcc -static-libstdc++ $(shell fltk-config --cxxflags)
	LDFLAGS += $(shell fltk-config --ldflags) -lws2_32
else
	CPPFLAGS += -lfltk
endif
SOURCES = main.cpp OO_FLTK.cpp ../ClientBase.cpp Client.cpp
OBJECTS = main.o OO_FLTK.o ClientBase.o Client.o
EXECUTABLE = WantChat

.PHONY: all clean

all: $(EXECUTABLE)

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CPP) $(CPPFLAGS) $(OBJECTS) $(LDFLAGS) -o $(EXECUTABLE)

$(OBJECTS):
	$(CPP) -c $(CPPFLAGS) $(SOURCES)