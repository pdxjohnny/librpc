#include <rpc.h>

// Makes requests based off the information provided in config
int rpc_client_start(struct rpc_client_config * config) {
    int err;
    int client;
    struct sockaddr_in server_addr;

    client = socket(AF_INET, SOCK_STREAM, 0);
    if (client == -1) {
        errno = ENOSOCK;
        return -1;
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
    struct hostent * server_info;
    server_info = gethostbyname(config->addr);
    if (!server_info) {
        close(client);
        return -1;
    }
    server_addr.sin_addr = *(struct in_addr *) server_info->h_addr;

    // Connect the client socket to the server
    err = connect(client, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (err == -1) {
        close(client);
        return -1;
    }

    config->client = client;

    return EXIT_SUCCESS;
}

int rpc_message_recv(struct rpc_message * msg) {
    int err;
    // So we know what to do with the response
    int res = RPC_HANDLE_SUCCESS;
    // Buffer to recv incoming data
    char buffer[RPC_MESSAGE_BUFFER_SIZE];
    // Put the client in here to see if it needs to be read from
    fd_set read_from;

    // Get rid of the old sockets
    FD_ZERO(&read_from);

    // Put the sockets we care about reading from into the read set
    FD_SET(msg->client, &read_from);

#ifdef RPC_LOGGING
    printf("rpc_message_recv: about to call select\n");
#endif

    // Wait for data to read
    err = select(msg->client + 1U, &read_from, NULL, NULL, 0);

#ifdef RPC_LOGGING
    printf("rpc_message_recv: returned from select\n");
#endif

    // Select will return -1 if it had an error
    if (err == -1) {
        errno = ESELECT;
        return -1;
    }

    // Read in the message that the client sent
    int bytes_read_last = 0;
    // Clear the buffer
    memset(buffer, 0, sizeof(buffer));
    // Read in the request leaving space to null terminate
    bytes_read_last = read(msg->client, buffer, RPC_MESSAGE_BUFFER_SIZE - 1);
    // NULL terminate the string
    buffer[bytes_read_last] = '\0';
    // Dont parse if there was no new data
    if (bytes_read_last < 1) {
        // Select said data but we got none, thats an error
        return -1;
    }
    // Parse the request
    rpc_message_parse(msg, buffer, bytes_read_last);

    // Mark the message as incomplete if we stoped reading before it was done
    // being parsed
    if (!msg->parse_complete) {
        msg->incomplete = 1;
    }

    // Got it all
    return EXIT_SUCCESS;
}
