CC=g++
CFLAGS=-c -static -I./include/
LDFLAGS=
SOURCES=$(wildcard src/*.cpp)
OBJECTS=$(addprefix obj/,$(notdir $(SOURCES:.cpp=.o)))
LIB=bin/rpc

all: $(SOURCES) $(LIB)

$(LIB): $(OBJECTS)
	@mkdir -p bin
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

obj/%.o: src/%.cpp
	@mkdir -p obj
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -fv obj/* bin/*

