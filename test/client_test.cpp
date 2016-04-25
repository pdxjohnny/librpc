#include "rpc-test.h"

int test_client_contact_server () {
    int err;

    // Configure the server
    uint16_t port = 0;
    char addr[] = "127.0.0.1";
    int comm[2];
    struct rpc_server_config server_config = {
        .addr = addr,
        .port = port,
        .handlers = NULL,
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
    // Make the request
    err = rpc_client(&client_config);
    if (err == -1) {
        // Report the exit status and exit
        printf("Error from client: %d\n", errno);
    }

    // Stop the server before we (the client) exit
    rpc_server_stop(&server_config);

    return EXIT_SUCCESS;
}

