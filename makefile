NAME=rpc
# Libs in use
LIB_NAMES=$(notdir $(wildcard deps/*))
LIB_HEADERS=$(addprefix -I./deps/,$(addsuffix /include/,$(LIB_NAMES)))
LIBS=$(addprefix ./deps/,$(join $(addsuffix /bin/,$(LIB_NAMES)),$(addsuffix .a,$(LIB_NAMES))))
LIB_NAMES_CLEAN=$(addprefix ./clean/,$(LIB_NAMES))
# Compiler settings
CC=gcc
TEST_CC=g++
AR=ar
ARFLAGS=rcs
CFLAGS=-g -fPIC -Wall -c -I./include/ $(LIB_HEADERS)
# Build the lib
LIB_SOURCES=$(wildcard src/*.c)
LIB_OBJECTS=$(addprefix obj/lib/,$(notdir $(LIB_SOURCES:.c=.o)))
LIB=bin/lib$(NAME).a
TAR=bin/lib$(NAME).tar.gz
HEADERS=$(wildcard include/*.h)
# Build the tests
LIB_NAMES_TEST=$(addprefix ./test/,$(LIB_NAMES))
TEST_SOURCES=$(wildcard test/*.cpp)
TEST_OBJECTS=$(addprefix obj/test/,$(notdir $(TEST_SOURCES:.cpp=.o)))
TEST=bin/$(NAME)-test
# Build the benchmarks
LIB_NAMES_BENCHMARK=$(addprefix ./benchmark/,$(LIB_NAMES))
BENCHMARK_SOURCES=$(wildcard benchmark/*.cpp)
BENCHMARK_OBJECTS=$(addprefix obj/benchmark/,$(notdir $(BENCHMARK_SOURCES:.cpp=.o)))
BENCHMARK=bin/$(NAME)-benchmark

all: $(LIB_NAMES) $(LIB_SOURCES) $(LIB) $(TEST_SOURCES) $(TEST) $(BENCHMARK_SOURCES) $(BENCHMARK)

$(LIB_NAMES):
	@# Make them all
	@$(foreach dep,$(addprefix ./deps/,$@),$(MAKE) -C $(dep))
	@# Extract them into their own obj directory so that they can be compiled
	@# into the archive for this library. Or else anything using this library
	@# also has to link to all of its dependencies
	@$(foreach dep,$(addprefix ./obj/deps/,$@),mkdir -p $(dep))
	@# Copy the archives in under the obj folder
	@$(foreach dep,$@, \
		cp \
			$(addprefix ./deps/,$(join $(addsuffix /bin/,$(dep)),$(addsuffix .a,$(dep)))) \
			$(addprefix ./obj/deps/,$(addsuffix /, $(dep))) \
	)
	@# Extract the archives rename them to have the archives name and delete
	@# the copy of the archive we copyied in
	@$(foreach dep,$@, \
		cd $(addprefix ./obj/deps/, $(dep)) && \
		$(AR) x $(addsuffix .a, $(dep)) && \
		rm $(addsuffix .a, $(dep)) && \
		for file in `ls`; do mv $$file ../$(dep)_$$file; done && \
		cd .. && \
		rm -rf $(dep) \
	)

tar: $(LIB)
	@tar --transform 's,include/,usr/include/,' \
		--transform 's,bin/,usr/lib/,' -czf $(TAR) $(LIB) $(HEADERS)

# Build the lib
$(LIB): $(LIB_OBJECTS) $(LIB_NAMES)
	@mkdir -p bin
	$(AR) $(ARFLAGS) $@ $(LIB_OBJECTS) $(wildcard ./obj/deps/*)

obj/lib/%.o: src/%.c
	@mkdir -p obj/lib
	$(CC) $(CFLAGS) $< -o $@

# Build tests
$(TEST): $(TEST_OBJECTS) $(LIB)
	@mkdir -p bin
	$(TEST_CC) $(LDFLAGS) $(TEST_OBJECTS) $(LIBS) $(LIB) -o $@

obj/test/%.o: test/%.cpp
	@mkdir -p obj/test
	$(TEST_CC) $(CFLAGS) $< -o $@

.PHONY: test
test: $(LIB_NAMES_TEST)
	@echo Running tests for $(NAME)
	@$(TEST)

$(LIB_NAMES_TEST):
	@$(foreach dep,$(addprefix ./deps/,$(notdir ,$@)),$(MAKE) -C $(dep) test)

# Build benchmarks
$(BENCHMARK): $(BENCHMARK_OBJECTS) $(LIB)
	@mkdir -p bin
	$(TEST_CC) $(LDFLAGS) $(BENCHMARK_OBJECTS) $(LIBS) $(LIB) -o $@

obj/benchmark/%.o: benchmark/%.cpp
	@mkdir -p obj/benchmark
	$(TEST_CC) $(CFLAGS) $< -o $@

.PHONY: benchmark
benchmark: $(LIB_NAMES_BENCHMARK)
	@echo Running benchmarks for $(NAME)
	@$(BENCHMARK)

$(LIB_NAMES_BENCHMARK):
	@$(foreach dep,$(addprefix ./deps/,$(notdir ,$@)),$(MAKE) -C $(dep) benchmark)

install:
	@tar xvf bin/lib$(NAME).tar.gz -C /

clean: $(LIB_NAMES_CLEAN)
	@rm -rf obj/* bin/*

$(LIB_NAMES_CLEAN):
	@$(foreach dep,$(addprefix ./deps/,$(notdir ,$@)),$(MAKE) -C $(dep) clean)

