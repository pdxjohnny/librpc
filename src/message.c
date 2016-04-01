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
    int err;

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
        return rpc_message_parse_http(msg, buffer, buffer_size);
    }

    return EXIT_SUCCESS;
}

// Parse an http mesage
int rpc_message_parse_http(struct rpc_message * msg, char * buffer, int buffer_size) {
    int err;

    // Make a string to hold the path
    char path[buffer_size + 1];
    memset(path, 0, sizeof(path));
    // The path will be right after the HTTP method (GET, POST, HEAD) so we
    // have to find out where the http method ends and the path begins
    char * path_start = strchr(buffer, ' ') + 1;
    // path_start will be NULL if it couldnt find a space in buffer
    if (path_start == NULL) {
        errno = EBADMSG;
        return -1;
    }

    // Grab the path so from after the method to the next space before HTTP/X.X
    err = rpc_string_untildelim(path, path_start, buffer_size + 1, ' ');
    if (err == -1) {
        return -1;
    }

    // For our purposess the path is what we call the method
    msg->method = rpc_string_on_heap(path, buffer_size + 1);
    // Make sure that memory allocation was a success
    if (msg->method == NULL) {
        return -1;
    }
    printf("msg->method \'%s\'\n", msg->method);

    return EXIT_SUCCESS;
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

