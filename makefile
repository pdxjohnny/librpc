CC=gcc
AR=ar
ARFLAGS=rcs
CFLAGS=-Wall -c -static -I./include/
SOURCES=$(wildcard src/*.c)
OBJECTS=$(addprefix obj/,$(notdir $(SOURCES:.c=.o)))
LIB=bin/librpc.a
TAR=bin/librpc.tar.gz
HEADERS=$(wildcard include/*.h)

all: $(SOURCES) $(LIB) $(TAR)

$(TAR): $(LIB)
	@tar --transform 's,include/,usr/include/,' \
		--transform 's,bin/,usr/lib/,' -czf $@ $(LIB) $(HEADERS)

$(LIB): $(OBJECTS)
	@mkdir -p bin
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

obj/%.o: src/%.c
	@mkdir -p obj
	$(CC) $(CFLAGS) $< -o $@

install:
	@tar xvf bin/librpc.tar.gz -C /

clean:
	@rm -f obj/* bin/*

