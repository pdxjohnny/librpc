CC=gcc
AR=ar
ARFLAGS=rcs
CFLAGS=-Wall -c -static -I./include/
SOURCES=$(wildcard src/*.c)
OBJECTS=$(addprefix obj/,$(notdir $(SOURCES:.c=.o)))
LIB=bin/librpc.a

all: $(SOURCES) $(LIB)

$(LIB): $(OBJECTS)
	@mkdir -p bin
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

obj/%.o: src/%.c
	@mkdir -p obj
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f obj/* bin/*

