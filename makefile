CC=g++
AR=ar
ARFLAGS=rcs
CFLAGS=-Wall -c -static -I./include/
SOURCES=$(wildcard src/*.cpp)
OBJECTS=$(addprefix obj/,$(notdir $(SOURCES:.cpp=.o)))
LIB=bin/librpc.a

all: $(SOURCES) $(LIB)

$(LIB): $(OBJECTS)
	@mkdir -p bin
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

obj/%.o: src/%.cpp
	@mkdir -p obj
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f obj/* bin/*

