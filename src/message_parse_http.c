#include "rpc.h"
#include <librpc.h>

// Parse an http mesage
int rpc_message_parse_http(struct rpc_message * msg, const char * buffer, int buffer_size) {
    int err;
    int parse_complete = 1;

    // Append the new data to the messages buffer until we have recived all the
    // headers
    rpc_message_append_to_buffer(msg, buffer, buffer_size);
    if (msg->length_recv > RPC_MSG_HTTP_MAX_HEADER_LENGTH) {
        return rpc_message_reply_http_413(msg);
    }

    // Make sure that we have at least all of the headers before we start
    // parsing the path and headers
    if (strstr(msg->buffer, "\r\n\r\n") == NULL) {
        return EXIT_SUCCESS;
    }

    // The buffer contains the headers so make headers equal to buffer
    msg->headers = msg->buffer;
    msg->length_headers = (uintptr_t)strstr(msg->headers, "\r\n\r\n") +
        (uintptr_t)4;
    msg->length_headers = MIN(msg->length_headers - (uintptr_t)msg->headers,
        (uintptr_t)msg->headers - msg->length_headers);
    msg->buffer = NULL;

    // We have all the headers so the buffer now needs to point to the end of
    // the headers
    if (msg->length_recv > msg->length_headers) {
        // The start of the data starts at the end of the headers which will be
        // a string which starts at the address at the end of the headers. Then
        // copy that into buffer because now we have started to buffer the body
        // of the message rather than the headers
        rpc_message_append_to_buffer(msg, &msg->headers[msg->length_headers],
            msg->length_recv - msg->length_headers);
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

    // Make sure there are headers
    if (msg->headers == NULL || msg->length_headers == 0) {
        errno = ENOMSG;
        return -1;
    }

    // Make a string to hold the path
    char path[msg->length_headers + 1];
    memset(path, 0, sizeof(path));
    // The path will be right after the HTTP method (GET, POST, HEAD) so we
    // have to find out where the http method ends and the path begins
    char * path_start = strchr(msg->headers, ' ') + 2;
    // path_start will be NULL if it couldnt find a space in msg->buffer
    if (path_start == NULL) {
        errno = EBADMSG;
        return -1;
    }

    // Try to go the the start of the urlencoded data or to the HTTP protocol
    // version so for the space it will grab the path from after the method to
    // the next space before HTTP/X.X
    int i;
    char tryEnds[] = "? ";
    for (i = 0; i < strlen(tryEnds); ++i) {
        err = rpc_string_untildelim(path_start, path, msg->length_headers + 1, tryEnds[i]);
        if (err == EXIT_SUCCESS) {
            break;
        }
    }
    // If neither worked then something it wrong
    if (err == -1) {
        return -1;
    }

    // For our purposess the path is what we call the method
    msg->method = rpc_string_on_heap(path, msg->length_headers + 1);
    // Make sure that memory allocation was a success
    if (msg->method == NULL) {
        return -1;
    }
    return EXIT_SUCCESS;
}

// Find a header
int rpc_message_parse_http_header(struct rpc_message * msg, const char * find, char * store, int store_max) {
    int err;
    char * header = NULL;

    // Make sure there are headers
    if (msg->headers == NULL || msg->length_headers == 0) {
        errno = ENOMSG;
        return -1;
    }

    // Check if the header is present ebfore trying to find it
    header = strstr(msg->headers, find);
    if (header == NULL) {
        errno = ENOMEDIUM;
        return -1;
    }

    // Go the the delim between the header and its value
    header = strchr(header, ':') + (uintptr_t)2;
    if (header == NULL) {
        errno = EBADMSG;
        return -1;
    }

    // Grab the value of the header
    err = rpc_string_untildelim(header, store, store_max, '\r');
    if (err == -1) {
        return -1;
    }
    return EXIT_SUCCESS;
}

// Parse the body for data
int rpc_message_parse_http_data(struct rpc_message * msg, const char * key, char * value, int max_value) {
    int err;

    // First check if the data is in the path
    err = rpc_message_parse_http_data_path(msg, key, value, max_value);
    if (err != 1) {
        return EXIT_SUCCESS;
    }

    // If we get to here we have not successfully found the data yet
    return -1;
}

// Look in the path for a data value
int rpc_message_parse_http_data_path(struct rpc_message * msg, const char * key, char * value, int max_value) {
    int err;

    // Make sure there are headers
    if (msg->headers == NULL || msg->length_headers == 0) {
        errno = ENOMSG;
        return -1;
    }

    // Look for the value in the path
    char * value_start = strstr(msg->headers, key);
    // value_start will be NULL if it couldnt find a question mark in
    // msg->buffer
    if (value_start == NULL) {
        errno = ENOMEDIUM;
        return -1;
    }

    // Look for the value in the path
    value_start = strchr(msg->headers, '=') + (uintptr_t)1;
    // value_start will be NULL if it couldnt find a question mark in
    // msg->buffer
    if (value_start == NULL) {
        errno = EBADMSG;
        return -1;
    }

    // Try to go the the start of the urlencoded data or to the HTTP protocol
    // version so for the space it will grab the path from after the method to
    // the next space before HTTP/X.X
    int i;
    char tryEnds[] = "& ";
    for (i = 0; i < strlen(tryEnds); ++i) {
        err = rpc_string_untildelim(value_start, value, max_value, tryEnds[i]);
        if (err == EXIT_SUCCESS) {
            return EXIT_SUCCESS;
        }
    }

    return -1;
}

