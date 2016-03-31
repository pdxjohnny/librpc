#include "rpc.h"
#include <librpc.h>

// Starts listening and calls handlers based on their applicability to the path
int rpc_start_server(struct rpc_server_config * config) {
    int err;
    int server, client;
    struct sockaddr_in server_addr, client_addr;
    socklen_t server_addr_size, client_addr_size;

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

    // Reset server_addr struct
    memset(&server_addr, 0, sizeof(server_addr));

    // Configure the server as specified in config
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = config->port;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind the socket
    err = bind(server, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (err == -1) {
        return ENOBIND;
    }

    // If we requested port 0 then whoever started the server probably wants to
    // know what port it is really on, so find out with getsockname
    server_addr_size = sizeof(server_addr);
    err = getsockname(server, (struct sockaddr *) &server_addr, &server_addr_size);
    if (err == -1) {
        return EPORT;
    }
    // Set the port in the config
    char port_string[12];
    sprintf(port_string, "%d", server_addr.sin_port);
    printf("Server port is %s\n", port_string);
    write(config->comm[RPC_COMM_WRITE], port_string, strlen(port_string));

    // Listen for incoming connections
    err = listen(server, SOMAXCONN);
    if (err == -1) {
        return ELISTEN;
    }

    // We are only conserned with reading from the stop pipe and the server
    // socket which gives us clients
    fd_set read_from;

    // Accept connections until we recv information from the pipe telling us to
    // do otherwise
    while (1) {
        // Get rid of the old sockets
        FD_ZERO(&read_from);

        // Put the sockets we care about reading from into the read set
        FD_SET(server, &read_from);
        FD_SET(config->comm[RPC_COMM_READ], &read_from);

        // Find the max file descriptor that we will be reading from
        int max_fd = MAX(config->comm[RPC_COMM_READ], server) + 1;

        // Now preform the select to see which gets data first
        // select returns the number of file descriptors that are ready to be
        // acted on that number id -1 if there was an error
        // last argument is zero because there is no need for a timeout just
        // wait indefinatly, this is all we care about
        err = select(max_fd, &read_from, NULL, NULL, 0);

        // Select will return -1 if it had an error
        if (err == -1) {
            return ESELECT;
        }

        // If the server socket is active then we need to accept the connection
        // that it is trying to give us
        if (FD_ISSET(server, &read_from)) {
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
        }

        // If there is something to read from comm that means we should
        // shutdown the server. Maybe in the future this will do other things
        // but for know sending anything into comm tells this server to
        // shutdown
        if (FD_ISSET(config->comm[RPC_COMM_READ], &read_from)) {
            // Read and print the message
            char buffer[20];
            read(config->comm[RPC_COMM_READ], buffer, 20);
            printf("Got shutdown message \'%s\'\n", buffer);
            // Shutdown the server
            shutdown(server, SHUT_RDWR);
            close(server);
            // Return 0 for all is well
            return 0;
        }

    }

    // We should never get to here
    return 0;
}

