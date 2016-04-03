#include "rpc.h"
#include <librpc.h>

// Parse an http mesage
int rpc_message_parse_http(struct rpc_message * msg, char * buffer, int buffer_size) {
    int err;
    int parse_complete = 1;

    // Append the new data to the messages buffer until we have recived all the
    // headers
    rpc_message_append_to_buffer(msg, buffer, buffer_size);

    // We have received and append to the buffer
    ++msg->recv_count;

    // Make sure that we have at least all of the headers before we start
    // parsing the path and headers
    if (strstr(msg->buffer, "\r\n\r\n") == NULL) {
        return EXIT_SUCCESS;
    }

    // We have all the headers but we might have gotten some of the body. So
    // lets make a pipe and feed all of the body that we got into the pipe.
    // That way the handler function reads directly from the body in case it
    // needs to read in a large amount of data without putting it in memory
    uintptr_t start_of_message = (uintptr_t)msg->buffer;
    int length_of_headers = (uintptr_t)strstr(msg->buffer, "\r\n\r\n") - 4 + start_of_message;
    if (msg->length_recv > length_of_headers) {
        // Create a pipe that will be the same as reading from the socket only we
        // are going to write the part of the body which we accidentaly capured
        // into it first
        int pipe_fd[2];
        pipe(pipe_fd);
        dup2(msg->client, pipe_fd[RPC_COMM_READ]);
    }

    // Parse the path which we will interperate as the method. If there is
    // urlencoded data sent in the path it will be added to data
    err = rpc_message_parse_http_path(msg);
    if (err == -1) {
        parse_complete = 0;
    }

    // Parse will be complete unless something failed then it will be set to
    // incomplete
    msg->parse_complete = parse_complete;

    return EXIT_SUCCESS;
}

// Prase the path
int rpc_message_parse_http_path(struct rpc_message * msg) {
    int err;

    // Make a string to hold the path
    char path[msg->length_recv + 1];
    memset(path, 0, sizeof(path));
    // The path will be right after the HTTP method (GET, POST, HEAD) so we
    // have to find out where the http method ends and the path begins
    char * path_start = strchr(msg->buffer, ' ') + 2;
    // path_start will be NULL if it couldnt find a space in msg->buffer
    if (path_start == NULL) {
        errno = EBADMSG;
        return -1;
    }

    // Grab the path so from after the method to the next space before HTTP/X.X
    err = rpc_string_untildelim(path, path_start, msg->length_recv + 1, ' ');
    if (err == -1) {
        return -1;
    }

    // For our purposess the path is what we call the method
    msg->method = rpc_string_on_heap(path, msg->length_recv + 1);
    // Make sure that memory allocation was a success
    if (msg->method == NULL) {
        return -1;
    }
    return EXIT_SUCCESS;
}

// Prase the headers
int rpc_message_parse_http_headers(struct rpc_message * msg) {
    return EXIT_SUCCESS;
}

// Parse the body for data
int rpc_message_parse_http_body(struct rpc_message * msg) {
    return EXIT_SUCCESS;
}

