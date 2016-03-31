#include "rpc.h"
#include <librpc.h>

// Makes requests based off the information provided in config
int rpc_client(struct rpc_client_config * config) {
    int err;
    int client;
    struct sockaddr_in server_addr;

    client = socket(AF_INET, SOCK_STREAM, 0);
    if (client == -1) {
        return ENOSOCK;
    }

    int yes = 1;
#ifdef SO_REUSEPORT
    setsockopt(client, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(yes));
#endif
#ifdef SO_REUSEADDR
    setsockopt(client, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
#endif

    // Reset server_addr struct
    memset(&server_addr, 0, sizeof(server_addr));

    // Make the server address as requested by config
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(config->port);
    inet_aton(config->addr, &server_addr.sin_addr);

    // Connect the client socket to the server
    err = connect(client, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (err == -1) {
        return ENOCONNECT;
    }

    // We want to know when the server is trying to send us data (read_from)
    // and when the server is ready for us to write data to it (write_to)
    fd_set read_from;
    fd_set write_to;

    // Accept connections until we recv information from the pipe telling us to
    // do otherwise
    while (1) {
        // Get rid of the old sockets
        FD_ZERO(&read_from);
        FD_ZERO(&write_to);

        // Check if our client can read from or write to the server
        FD_SET(client, &read_from);
        FD_SET(client, &write_to);

        // Find the max file descriptor that we will be reading from
        // As of know we only have the one for client
        int max_fd = MAX(0, client) + 1;

        // Now preform the select to see which gets data first
        // select returns the number of file descriptors that are ready to be
        // acted on that number id -1 if there was an error
        // last argument is zero because there is no need for a timeout just
        // wait indefinatly, this is all we care about we need a response
        err = select(max_fd, &read_from, &write_to, NULL, 0);

        // Select will return -1 if it had an error
        if (err == -1) {
            return ESELECT;
        }

        // If the server is trying to write data to us our client will be in
        // the read_from array
        if (FD_ISSET(client, &read_from)) {
            // Read 100 bytes from the server
            char buffer[100];
            // Keep track of how many bytes we actually got from the server
            int n_recv;
            n_recv = read(client, buffer, 100);
            // Display the message (just for now)
            printf("Got %d bytes from server \'%s\'\n", n_recv, buffer);
            // Close the connection with the server
            close(client);
            return 1;
        }
        // If the server is waiting for use to send it data then it will be in
        // the write_to array
        if (FD_ISSET(client, &write_to)) {
            // Send the server some information
            char msg[] = "Hello World";
            send(client, msg, strlen(msg), 0);
            // Close the connection with the server
            close(client);
            return 2;
        }


    }

    // We should never get to here
    return 0;
}

