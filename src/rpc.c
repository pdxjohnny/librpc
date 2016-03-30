#include "rpc.h"
#include <librpc.h>

int rpc_test () {
    return 42;
}

// Search the response for field and store it in ret
int rpc_field(char * field, char * ret, struct rpc_message * msg) {
    return 0;
};


// Starts listening and calls handlers based on their applicability to the path
int rpc_start_server(struct rpc_handler * handlers) {
    int err;
    int server, client;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_size;

    server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == -1) {
        return ENOSOCK;
    }

    int yes = 1;
#ifdef SO_REUSEPORT
    setsockopt(server, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(yes));
#endif
#ifdef SO_REUSEADDR
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
#endif

    // Reset sockaddr_un struct
    memset(&server_addr, 0, sizeof(server_addr));

    // Bind the socket
    err = bind(server, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (err == -1) {
        return ENOBIND;
    }

    // Listen for incoming connections
    err = listen(server, SOMAXCONN);
    if (err == -1) {
        return ELISTEN;
    }

    // Accept connections as they come in
    client_addr_size = sizeof(client_addr);
    client = accept(server, (struct sockaddr *) &client_addr, &client_addr_size);
    if (client == -1) {
        return EACCEPT;
    }

    // Send the client some information
    char msg[] = "Hello World";
    send(client, msg, strlen(msg), 0);

    // Close the connection with the client
    close(client);

    // Shutdown the server
    close(server);

    return 0;
}

