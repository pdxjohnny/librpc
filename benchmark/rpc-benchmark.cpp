#include "rpc-benchmark.h"

int main (int argc, char ** argv, char ** env) {
    int err;

#ifdef RPC_BENCHMARK_SERVER
    // Server Benchmarks
    RPC_BENCHMARK(benchmark_server_connections_per_minute);
#endif

    return EXIT_SUCCESS;
}

