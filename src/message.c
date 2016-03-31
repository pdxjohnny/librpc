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
    msg->headers = NULL;
    msg->data = NULL;
    msg->body = NULL;
    msg->raw = NULL;

    return EXIT_SUCCESS;
}

// Parse the mesage
int rpc_message_parse(struct rpc_message * msg, char * buffer, int buffer_size) {
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

