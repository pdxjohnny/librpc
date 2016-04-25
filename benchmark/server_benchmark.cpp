#include "rpc-benchmark.h"

int benchmark_server_connections_per_minute() {
    int err;

    // Configure the server
    uint16_t port = 0;
    char addr[] = "127.0.0.1";
    struct rpc_handler handlers[] = {
        NULL
    };
    int comm[2];
    struct rpc_server_config server_config = {
        .addr = addr,
        .port = port,
        .handlers = handlers,
        .not_found = NULL,
        .comm = comm
    };

    // Configure the client
    struct rpc_client_config client_config = {
        .addr = addr,
        .port = 0,
    };

    // Start the server in the background
    rpc_server_start_background(&server_config);

    // Set the client port to be what the server reports its port is
    client_config.port = server_config.port;

    // Count number of connections
    int num_con = 0;

    // Get the time before requests start
    time_t start, end;
    start = time(NULL);

    do {
        // Make the request
        err = rpc_client(&client_config);
        if (err == -1) {
            // Report the exit status and exit
            printf("Error from client: %d\n", errno);
        }
        // Update num_con
        ++num_con;
        // Update end time
        end = time(NULL);
    } while (difftime(end, start) < 1.00);

    // Time spent
    double diff = difftime(end, start);

    // Report
    printf("benchmark_server_connections_per_minute: %d connections in %0.02f seconds\n", num_con, diff);

    // Stop the server before we (the client) exit
    rpc_server_stop(&server_config);

    return EXIT_SUCCESS;
}

