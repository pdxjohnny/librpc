#include <rpc.h>

int main (int argc, char **argv) {
    int err;

    uint16_t port = 0;
    if (argc > 1) {
        port = strtoul(argv[1], NULL, 10);
    }

    // Configure the server
    char addr[] = "0.0.0.0";
    struct rpc_server_config server_config = {
       /* .addr = */ addr,
       /* .port = */ port,
       /* .handlers = */ NULL,
       /* .not_found = */ NULL,
       /* .comm = */ NULL
    };

    // Start the server
    return rpc_server_start(&server_config);
}
