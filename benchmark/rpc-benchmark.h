#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#include <librpc.h>

// Which benchmarks to run
#define RPC_BENCHMARK_SERVER

#define RPC_BENCHMARK(benchmark_name) \
({\
    err = benchmark_name();\
    if (err == -1) {\
        printf(#benchmark_name " exited with status %d\n", errno);\
    }\
})


// Server Benchmarks
int benchmark_server_connections_per_minute();

