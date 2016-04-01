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
    msg->method = NULL;
    msg->headers = NULL;
    msg->data = NULL;
    msg->body = NULL;
    msg->raw = NULL;

    return EXIT_SUCCESS;
}

// Parse the mesage
int rpc_message_parse(struct rpc_message * msg, char * buffer, int buffer_size) {
    char * first_newline = NULL;
    // Check the first line of the message
    first_newline = strchr(buffer, '\n');
    // first_newline will be NULL if it couldnt find a newline in the buffer
    if (first_newline == NULL) {
        errno = EBADMSG;
        return -1;
    }
    // Find out how many characters in between the begining and the newline
    uintptr_t first_line_length = first_newline - buffer;
    // That number should never be longer than the size of the whole buffer
    printf("first line is %ld long\n", first_line_length);
    if (first_line_length > buffer_size) {
        errno = ERANGE;
        return -1;
    }
    // Make a string to hold the first line + the null terminating character
    char first_line[first_line_length + 1];
    // Copy the first line fromt he buffer into its own string
    memcpy(first_line, buffer, first_line_length);
    // NULL terminate the first line string
    first_line[first_line_length + 1] = '\0';

    // Go though all the protocols that we know how to parse and parse the
    // message if that protocol type is present in the first line
    if (strstr(first_line, "HTTP")) {
        return rpc_message_parse_http(msg, buffer, buffer_size);
    }

    return EXIT_SUCCESS;
}

// Parse an http mesage
int rpc_message_parse_http(struct rpc_message * msg, char * buffer, int buffer_size) {
    printf("HTTP PARSE %s", buffer);
    return EXIT_SUCCESS;
}

// Free the message when we are done with it
int rpc_message_free(struct rpc_message * msg) {
    return EXIT_SUCCESS;
}

// Search the response for field and store it in ret
int rpc_field(char * field, char * ret, struct rpc_message * msg) {
    return EXIT_SUCCESS;
};

