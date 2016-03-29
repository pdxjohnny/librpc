CC=g++
CFLAGS=-c
LDFLAGS=
SOURCES=$(wildcard src/*.cpp)
OBJECTS=$(addprefix obj/,$(notdir $(CPP_FILES:.cpp=.o)))
LIB=bin/rpc.a

all: $(SOURCES) $(LIB)

$(LIB): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o a.out

