#include "rpc.h"
#include <librpc.h>

int rpc_test () {
    return 42;
}

// Initializes an rpc_message
int rpc_message_init(struct rpc_message * msg) {
    msg->length = 0;
    msg->length_recv = 0;
    msg->parse_complete = 0;
    msg->incomplete = 0;
    msg->protocol = RPC_PROTOCOL_UNKNOWN;
    msg->method = NULL;
    msg->headers = NULL;
    msg->data = NULL;
    msg->body = NULL;
    msg->raw = NULL;

    return EXIT_SUCCESS;
}

// Parse the mesage
int rpc_message_parse(struct rpc_message * msg, char * buffer, int buffer_size) {
    int err;

    // Check if we already know what kind of message this is
    if (msg->protocol != RPC_PROTOCOL_UNKNOWN) {
        return rpc_message_parse_protocol(msg, buffer, buffer_size);
    }

    // Make a string to hold the first line + the null terminating character
    // It needs to be at least as long as buffer_size in case there is no
    // newline on purpose
    char first_line[buffer_size + 1];
    // Grab the first string until the newline
    err = rpc_string_untildelim(first_line, buffer, buffer_size + 1, '\n');
    if (err == -1) {
        return -1;
    }

    // Go though all the protocols that we know how to parse and parse the
    // message if that protocol type is present in the first line
    if (strstr(first_line, "HTTP")) {
        msg->protocol = RPC_PROTOCOL_HTTP;
    }

    return rpc_message_parse_protocol(msg, buffer, buffer_size);
}

// Pick the correct parser for the message
int rpc_message_parse_protocol(struct rpc_message * msg, char * buffer, int buffer_size) {
    switch (msg->protocol) {
    case RPC_PROTOCOL_HTTP:
        return rpc_message_parse_http(msg, buffer, buffer_size);
    }

    errno = ENOPROTOOPT;
    return -1;
}

// Free the message when we are done with it
int rpc_message_free(struct rpc_message * msg) {
    if (msg->method != NULL) {
        free(msg->method);
        msg->method = NULL;
    }
    return EXIT_SUCCESS;
}

// Search the response for field and store it in ret
int rpc_field(char * field, char * ret, struct rpc_message * msg) {
    return EXIT_SUCCESS;
};

