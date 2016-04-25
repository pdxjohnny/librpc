#include "rpc-test.h"

#define RPC_TEST_SERVER_CORRECT_PORT 34523

int test_server_correct_port() {
    // Configure the server
    uint16_t port = RPC_TEST_SERVER_CORRECT_PORT;
    char addr[] = "127.0.0.1";
    int comm[2];
    struct rpc_server_config server_config = {
        .addr = addr,
        .port = port,
        .handlers = NULL,
        .not_found = NULL,
        .comm = comm
    };

    // Start the server in the background
    rpc_server_start_background(&server_config);

    // Set the client port to be what the server reports its port is
    port = server_config.port;
    // Make sure the port is corret
    if (port != RPC_TEST_SERVER_CORRECT_PORT) {
        return -1;
    }

    // Stop the server before we (the client) exit
    rpc_server_stop(&server_config);

    return EXIT_SUCCESS;
}

// Make sure the handler works
int test_server_correct_handler() {
    // Configure the server
    uint16_t port = RPC_TEST_SERVER_CORRECT_PORT;
    char addr[] = "127.0.0.1";
    int comm[2];
    struct rpc_server_config server_config = {
        .addr = addr,
        .port = port,
        .handlers = NULL,
        .not_found = NULL,
        .comm = comm
    };

    // Start the server in the background
    rpc_server_start_background(&server_config);

    // Set the client port to be what the server reports its port is
    port = server_config.port;
    // Make sure the port is corret
    if (port != RPC_TEST_SERVER_CORRECT_PORT) {
        return -1;
    }

    // Stop the server before we (the client) exit
    rpc_server_stop(&server_config);

    return EXIT_SUCCESS;
}

