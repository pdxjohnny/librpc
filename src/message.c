#include "rpc.h"
#include <librpc.h>

int rpc_test () {
    return 42;
}

// Initializes an rpc_message
int rpc_message_init(struct rpc_message * msg) {
    msg->length = 0;
    msg->length_recv = 0;
    msg->length_headers = 0;
    msg->recv_count = 0;
    msg->client = 0;
    msg->parse_complete = 0;
    msg->incomplete = 0;
    msg->protocol = RPC_PROTOCOL_UNKNOWN;
    msg->method = NULL;
    msg->headers = NULL;
    msg->data = NULL;
    msg->buffer = NULL;

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
    if (msg->buffer != NULL) {
        free(msg->buffer);
        msg->buffer = NULL;
    }
    if (msg->headers != NULL) {
        free(msg->headers);
        msg->headers = NULL;
    }
    return EXIT_SUCCESS;
}

// Append data in buffer to data previously recived stored in the messages
// buffer
int rpc_message_append_to_buffer(struct rpc_message * msg, char * buffer, int buffer_size) {
    // If this is the first time appending data to the messages buffer then
    // we need to allocate a buffer for the message and place this initial data
    // in it rather than copying existing data nd new data into a new buffer
    if (msg->buffer == NULL) {
        // Allocate the messages initial buffer
        msg->buffer = rpc_string_on_heap(buffer, buffer_size);
        // We have appended the new data to the buffer
        msg->length_recv = buffer_size;
    } else {
        // If we are recving more data and this is not the first time then we
        // need to append to the buffer
        char new_buffer[msg->length_recv + buffer_size];
        memset(new_buffer, 0, sizeof(new_buffer));
        // Copy the old buffer into the new buffer
        strncpy(new_buffer, msg->buffer, msg->length_recv);
        // Free the old buffer now that we have copied it into the new one
        free(msg->buffer);
        // Copy the current buffer into the new buffer
        strncpy(new_buffer + (uintptr_t)msg->length_recv, buffer, buffer_size);
        // Allocate the new buffer on the heap
        msg->buffer = rpc_string_on_heap(new_buffer, msg->length_recv + buffer_size);
        // We have appended the new data to the buffer
        msg->length_recv += buffer_size;
    }

    return EXIT_SUCCESS;
}

// Search the response for field and store it in ret
int rpc_field(char * field, char * ret, struct rpc_message * msg) {
    return EXIT_SUCCESS;
};

