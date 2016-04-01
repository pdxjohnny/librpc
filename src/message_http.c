#include "rpc.h"
#include <librpc.h>

// Parse an http mesage
int rpc_message_parse_http(struct rpc_message * msg, char * buffer, int buffer_size) {
    int err;

    // Make a string to hold the path
    char path[buffer_size + 1];
    memset(path, 0, sizeof(path));
    // The path will be right after the HTTP method (GET, POST, HEAD) so we
    // have to find out where the http method ends and the path begins
    char * path_start = strchr(buffer, ' ') + 2;
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
    msg->parse_complete = 1;

    return EXIT_SUCCESS;
}

