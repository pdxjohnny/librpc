#include <rpc.h>

int main (int argc, char **argv) {
    int err;

    uint16_t port = 0;
    if (argc > 1) {
        port = strtoul(argv[1], NULL, 10);
    }

    // Configure the server
    char addr[] = "0.0.0.0";
    struct rpc_handler handlers[] = {
        NULL
    };
    int comm[2];
    struct rpc_server_config server_config = {
       /* .addr = */ addr,
       /* .port = */ port,
       /* .handlers = */ handlers,
       /* .not_found = */ NULL,
       /* .comm = */ comm
    };

    // Configure the client
    struct rpc_client_config client_config = {
       /* .addr = */ addr,
       /* .port = */ 0,
    };

    // Start the server in the background
    rpc_server_start_background(&server_config);

    // Let the user know what port we are serving on
    printf("Serving on %s:%d\n", server_config.addr, server_config.port);

    // Sleep until we recive a signal
    while (sleep(10000) == 0) {}

    // Stop the server before we (the client) exit
    rpc_server_stop(&server_config);

    return EXIT_SUCCESS;
}

