#include "rpc.h"
#include <librpc.h>


// Starts listening and calls handlers based on their applicability to the path
int rpc_server_start(struct rpc_server_config * config) {
    int err;
    int server, client;
    struct sockaddr_in server_addr, client_addr;
    socklen_t server_addr_size, client_addr_size;

    server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == -1) {
        errno = ENOSOCK;
        return -1;
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
    server_addr.sin_port = htons(config->port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind the socket
    err = bind(server, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (err == -1) {
        errno = ENOBIND;
        return -1;
    }

    // If we requested port 0 then whoever started the server probably wants to
    // know what port it is really on, so find out with getsockname
    server_addr_size = sizeof(server_addr);
    err = getsockname(server, (struct sockaddr *) &server_addr, &server_addr_size);
    if (err == -1) {
        errno = EPORT;
        return -1;
    }
    // Set the port in the config
    uint16_t port;
    char port_string[12];
    memset(port_string, 0, sizeof(port_string));
    // Grab the port it will be in network byte order
    sprintf(port_string, "%d", server_addr.sin_port);
    // Put that network byte order port into a int
    port = atoi(port_string);
    // Now we can convert it back to host byte order
    port = ntohs(port);
    // Send that port back to whatever started the server
    sprintf(port_string, "%d", port);
    write(config->comm[RPC_COMM_WRITE], port_string, strlen(port_string));

    // Listen for incoming connections
    err = listen(server, SOMAXCONN);
    if (err == -1) {
        errno = ELISTEN;
        return -1;
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
            errno = ESELECT;
            return -1;
        }

        // If the server socket is active then we need to accept the connection
        // that it is trying to give us
        if (FD_ISSET(server, &read_from)) {
            // Accept connections as they come in
            client_addr_size = sizeof(client_addr);
            client = accept(server, (struct sockaddr *) &client_addr, &client_addr_size);
            if (client == -1) {
                errno = EACCEPT;
                return -1;
            }
            // Handle the client
            err = rpc_server_handle_client(config, &client_addr, client);
            if (client == -1) {
                errno = EACCEPT;
                return -1;
            }
        }

        // If there is something to read from comm that means we should
        // shutdown the server. Maybe in the future this will do other things
        // but for know sending anything into comm tells this server to
        // shutdown
        if (FD_ISSET(config->comm[RPC_COMM_READ], &read_from)) {
            // Read and print the message
            char buffer[20];
            // Initialize the buffer to NULL
            memset(buffer, 0, 20);
            read(config->comm[RPC_COMM_READ], buffer, 20);
            // Shutdown the server
            shutdown(server, SHUT_RDWR);
            close(server);
            // Data was send so close the write end of the pipe
            close(config->comm[RPC_COMM_WRITE]);
            // Server is closed so we dont need the read end of the pipe anymore
            close(config->comm[RPC_COMM_READ]);
            // Exit 0 for all is well
            exit(EXIT_SUCCESS);
        }

    }

    // We should never get to here
    return EXIT_SUCCESS;
}

// Start the server in the background
int rpc_server_start_background(struct rpc_server_config * config) {
    int err;
    char buffer[RPC_GET_PORT_BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));

    // Create a pipe so we can tell the server when to stop running
    // Read end of the pipe is index 0 write end is index 1
    int pipe_stop[2];
    err = pipe(pipe_stop);
    if (err == -1) {
        errno = ECREATECOMM;
        return -1;
    }

    // Create a pipe so that the server can report its port to us
    // Read end of the pipe is index 0 write end is index 1
    int pipe_port[2];
    err = pipe(pipe_port);
    if (err == -1) {
        errno = ECREATECOMM;
        return -1;
    }

    // New array for server comm
    int server_comm[2];

    // Fork so that the server is running in the background
    switch (fork()) {
    case -1:
        // Error
        errno = EBACKGROUND;
        return -1;

    case 0:
        // Clild
        // Set the comm ports so that the server can write to the port pipe and
        // read from the close pipe.
        server_comm[RPC_COMM_READ] = pipe_stop[RPC_COMM_READ];
        server_comm[RPC_COMM_WRITE] = pipe_port[RPC_COMM_WRITE];
        config->comm = server_comm;
        // Start the server dont worry about capturing the exit because we have
        // no way to send it to the parent as of now
        return rpc_server_start(config);

    default:
        // Parent
        config->comm[RPC_COMM_WRITE] = pipe_stop[RPC_COMM_WRITE];
        config->comm[RPC_COMM_READ] = pipe_port[RPC_COMM_READ];
        // Read the port from the pipe
        read(pipe_port[RPC_COMM_READ], buffer, RPC_GET_PORT_BUFFER_SIZE);
        // Set the client port to be what the server reports its port is
        // Convert the string to a port number
        config->port = atoi(buffer);
        return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;
}

// Stop the server gracefully by sending the stop command through the pipe
int rpc_server_stop(struct rpc_server_config * config) {
    // Send some data trough, the server will stop once we do
    char stop_msg[] = "stop";
    write(config->comm[RPC_COMM_WRITE], stop_msg, strlen(stop_msg));
    // Data was send so close the write end of the pipe
    close(config->comm[RPC_COMM_WRITE]);
    // Server is closed so we dont need the read end of the pipe anymore
    close(config->comm[RPC_COMM_READ]);
    return EXIT_SUCCESS;
}

// Called on new client accept
int rpc_server_handle_client(struct rpc_server_config * config, struct sockaddr_in * client_addr, int client) {
    // Message we are receiving
    struct rpc_message msg;
    rpc_message_init(&msg);
    msg.client = client;
    // Buffer to recv incoming data
    char buffer[RPC_MESSAGE_BUFFER_SIZE];

    // Read in the message that the client sent
    int bytes_read_last = 0;
    do {
        // Clear the buffer
        memset(buffer, 0, sizeof(buffer));
        // Read in the request
        bytes_read_last = read(client, buffer, RPC_MESSAGE_BUFFER_SIZE);
        // Dont parse if there was no new data
        if (bytes_read_last < 1) {
            break;
        }
        // Parse the request
        rpc_message_parse(&msg, buffer, bytes_read_last);
        // Done parsing
    } while (!msg.parse_complete);

    // Mark the message as incomplete if we stoped reading before it was done
    // being parsed
    if (!msg.parse_complete) {
        msg.incomplete = 1;
    }

    // Call whatever handler is appropriate
    rpc_server_reply_client(config, client_addr, client, &msg);
    // Send the client some information
    // char msg[] = "Hello World";
    // send(client, msg, strlen(msg), 0);

    // Get rid of the message now that we are done with this client
    rpc_message_free(&msg);

    // Close the connection with the client
    close(client);

    return EXIT_SUCCESS;
}

// Pick the correct method to use in our reply to the client
int rpc_server_reply_client(struct rpc_server_config * config, struct sockaddr_in * client_addr, int client, struct rpc_message * msg) {
    // Make sure there are some handlers to check
    struct rpc_handler * handler;
    if (config->handlers == NULL && config->not_found == NULL) {
        // Couldnt find that method
        errno = ENOSYS;
        return -1;
    }

#ifdef RPC_LOGGING
    printf("Responding to method \"%s\"\n", msg->method);
#endif

    // Look through all of the handlers and choose the appropriate one to call
    // and return. The handler list should be terminated with NULL
    for (handler = config->handlers; *((uintptr_t *)handler) != (uintptr_t)NULL; ++handler) {
        if (0 == strncmp(msg->method, handler->method, strlen(handler->method))) {
            return handler->func(client, msg);
        }
    }

    // Couldnt find that method so call the not_found handler
    return config->not_found(client, msg);
}

