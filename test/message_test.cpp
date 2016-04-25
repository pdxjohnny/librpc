#include "rpc-test.h"

const char get_request[] =
        "GET /somemethod?data=value HTTP/1.1\r\n"
        "Host: 127.0.0.1:45311\r\n"
        "Connection: keep-alive\r\n"
        "Accept: text/html,application/xhtml+x�ml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"
        "Upgrade-Insecure-Requests: 1\r\n"
        "User-Agent: Mozilla/5.0� (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/49.0.2623.110 Safari/537.36\r\n"
        "DNT:� 1\r\n"
        "Accept-Encoding: gzip, deflate, sdch\r\n"
        "Accept-Language: en-US,en;q=0.8\r\n"
        "\r\n";

const char post_request[] =
        "GET /somemethod HTTP/1.1\r\n"
        "Host: 127.0.0.1:45311\r\n"
        "Connection: keep-alive\r\n"
        "Accept: text/html,application/xhtml+x�ml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"
        "Upgrade-Insecure-Requests: 1\r\n"
        "User-Agent: Mozilla/5.0� (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/49.0.2623.110 Safari/537.36\r\n"
        "DNT:� 1\r\n"
        "Accept-Encoding: gzip, deflate, sdch\r\n"
        "Accept-Language: en-US,en;q=0.8\r\n"
        "Content-Length: 10\r\n"
        "\r\n"
        "data=value";

int test_message_malloc_free_buffer() {
    char request_1[] =
        "GET /somemethod HTTP/1.1\r\n"
        "Host: 127.0.0.1:45311\r\n";
    char request_2[] =
        "Connection: keep-alive\r\n"
        "Accept: text/html,application/xhtml+x�ml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n";
    char request_3[] =
        "Upgrade-Insecure-Requests: 1\r\n"
        "User-Agent: Mozilla/5.0� (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/49.0.2623.110 Safari/537.36\r\n";
    char request_4[] =
        "DNT:� 1\r\n"
        "Accept-Encoding: gzip, deflate, sdch\r\n"
        "Accept-Language: en-US,en;q=0.8\r\n"
        "\r\n";

    // Create the message to parse into
    struct rpc_message msg;
    rpc_message_init(&msg);

    // Try parsing the request
    rpc_message_parse(&msg, request_1, strlen(request_1));
    rpc_message_parse(&msg, request_2, strlen(request_2));
    rpc_message_parse(&msg, request_3, strlen(request_3));
    rpc_message_parse(&msg, request_4, strlen(request_4));

    // Make sure that the message was parsed correctly
    RPC_TEST_EQ(msg.protocol, RPC_PROTOCOL_HTTP);
    RPC_TEST_STR_EQ(msg.method, "somemethod");

    // Free the message
    rpc_message_free(&msg);

    return EXIT_SUCCESS;
}

int test_message_parse() {
    // Create the message to parse into
    struct rpc_message msg;
    rpc_message_init(&msg);

    // Try parsing the request
    rpc_message_parse(&msg, get_request, strlen(get_request));

    // Make sure that the message was parsed correctly
    RPC_TEST_EQ(msg.protocol, RPC_PROTOCOL_HTTP);
    RPC_TEST_STR_EQ(msg.method, "somemethod");

    // Free the message
    rpc_message_free(&msg);

    return EXIT_SUCCESS;
}

int test_rpc_message_parse_http_path() {
    int err;

    // Create the message to parse into
    struct rpc_message msg;
    rpc_message_init(&msg);

    // Headers on the stack for simplicity
    char headers_1[] = "GET /path HTTP/1.1\r\n";
    msg.headers = headers_1;
    msg.length_headers = strlen(msg.headers);

    // Try parsing the request
    err = rpc_message_parse_http_path(&msg);
    if (err == -1) {
        return err;
    }

    // Make sure that the message was parsed correctly
    RPC_TEST_STR_EQ(msg.method, "path");

    // Free the method
    free(msg.method);

    // Headers on the stack for simplicity
    char headers_2[] = "GET /path?data=value HTTP/1.1\r\n";
    msg.headers = headers_2;
    msg.length_headers = strlen(msg.headers);

    // Try parsing the request
    err = rpc_message_parse_http_path(&msg);
    if (err == -1) {
        return err;
    }

    // Make sure that the message was parsed correctly
    RPC_TEST_STR_EQ(msg.method, "path");

    // So that we dont try to free the headers
    msg.headers = NULL;

    // Free the message
    rpc_message_free(&msg);

    return EXIT_SUCCESS;
}

int test_rpc_message_parse_http_header() {
    int err;
    char header[100];

    // Create the message to parse into
    struct rpc_message msg;
    rpc_message_init(&msg);

    // Headers on the stack for simplicity
    msg.headers = (char *)get_request;
    msg.length_headers = strlen(msg.headers);

    // Try parsing the request
    memset(header, 0, sizeof(header));
    err = rpc_message_parse_http_header(&msg, "Host", header, 100);
    if (err == -1) {
        return err;
    }

    // Make sure that the message was parsed correctly
    RPC_TEST_STR_EQ(header, "127.0.0.1:45311");

    // Try parsing the request
    memset(header, 0, sizeof(header));
    err = rpc_message_parse_http_header(&msg, "Connection", header, 100);
    if (err == -1) {
        return err;
    }

    // Make sure that the message was parsed correctly
    RPC_TEST_STR_EQ(header, "keep-alive");

    // Try parsing the request
    memset(header, 0, sizeof(header));
    err = rpc_message_parse_http_header(&msg, "Accept-Encoding", header, 100);
    if (err == -1) {
        return err;
    }

    // Make sure that the message was parsed correctly
    RPC_TEST_STR_EQ(header, "gzip, deflate, sdch");

    // So that we dont try to free the headers
    msg.headers = NULL;

    // Free the message
    rpc_message_free(&msg);

    return EXIT_SUCCESS;
}

int test_rpc_message_parse_http_data() {
    // Create the message
    struct rpc_message msg;
    rpc_message_init(&msg);

    // Try parsing the request
    rpc_message_parse_http(&msg, post_request, strlen(post_request));

    // Make sure the right message was sent
    RPC_TEST_STR_EQ(msg.buffer, "data=value");

    // Free the message
    rpc_message_free(&msg);

    return EXIT_SUCCESS;
}

int test_rpc_message_parse_http_headers_too_long() {
    char buffer[RPC_MSG_HTTP_MAX_HEADER_LENGTH + 1];
    int i;
    for (i = 0; i <= RPC_MSG_HTTP_MAX_HEADER_LENGTH; ++i) {
        buffer[i] = 'a';
    }
    buffer[RPC_MSG_HTTP_MAX_HEADER_LENGTH + 1] = '\0';

    // Create the message
    struct rpc_message msg;
    rpc_message_init(&msg);

    // Message needs a "socket" to write response to
    int pipe_fd[2];
    pipe(pipe_fd);
    msg.client = pipe_fd[RPC_COMM_WRITE];

    // Try parsing the request
    rpc_message_parse_http(&msg, buffer, RPC_MSG_HTTP_MAX_HEADER_LENGTH + 1);

    // Read the response
    char response[200];
    memset(response, 0, sizeof(response));
    read(pipe_fd[RPC_COMM_READ], response, 200);

    // Make sure the right message was sent
    RPC_TEST_STR_EQ(response, RPC_REPLY_HTTP_413);

    // Free the message
    rpc_message_free(&msg);

    return EXIT_SUCCESS;
}

int test_rpc_message_parse_http_data_path() {
    int err;

    // Create the message to parse into
    struct rpc_message msg;
    rpc_message_init(&msg);

    // Headers on the stack for simplicity
    char headers_2[] = "GET /path?data=value HTTP/1.1\r\n";
    msg.headers = headers_2;
    msg.length_headers = strlen(msg.headers);

    // Try parsing the request
    char value[20];
    memset(value, 0, sizeof(value));
    err = rpc_message_parse_http_data_path(&msg, "data", value, sizeof(value));
    if (err == -1) {
        return err;
    }

    // Make sure that the message was parsed correctly
    RPC_TEST_STR_EQ(value, "value");

    // So that we dont try to free the headers
    msg.headers = NULL;

    // Free the message
    rpc_message_free(&msg);

    return EXIT_SUCCESS;
}

int test_rpc_field() {
    int err;

    // Create the message to parse into
    struct rpc_message msg;
    rpc_message_init(&msg);
    msg.protocol = RPC_PROTOCOL_HTTP;

    // Headers on the stack for simplicity
    char headers_2[] = "GET /path?data=value HTTP/1.1\r\n";
    msg.headers = headers_2;
    msg.length_headers = strlen(msg.headers);

    // Try parsing the request
    char key[] = "data";
    char value[20];
    memset(value, 0, sizeof(value));
    err = rpc_field(&msg, key, value, sizeof(value));
    if (err == -1) {
        return err;
    }

    // Make sure that the message was parsed correctly
    RPC_TEST_STR_EQ(value, "value");

    // So that we dont try to free the headers
    msg.headers = NULL;

    // Free the message
    rpc_message_free(&msg);

    return EXIT_SUCCESS;
}

int test_rpc_field_int() {
    int err;

    // Create the message to parse into
    struct rpc_message msg;
    rpc_message_init(&msg);
    msg.protocol = RPC_PROTOCOL_HTTP;

    // Headers on the stack for simplicity
    char headers_2[] = "GET /path?data=42 HTTP/1.1\r\n";
    msg.headers = headers_2;
    msg.length_headers = strlen(msg.headers);

    // Try parsing the request
    char key[] = "data";
    int value;
    err = rpc_field_int(&msg, key, &value, 20);
    if (err == -1) {
        return err;
    }

    // Make sure that the message was parsed correctly
    RPC_TEST_EQ(value, 42);

    // So that we dont try to free the headers
    msg.headers = NULL;

    // Free the message
    rpc_message_free(&msg);

    return EXIT_SUCCESS;
}

int test_rpc_field_float() {
    int err;

    // Create the message to parse into
    struct rpc_message msg;
    rpc_message_init(&msg);
    msg.protocol = RPC_PROTOCOL_HTTP;

    // Headers on the stack for simplicity
    char headers_2[] = "GET /path?data=4.2 HTTP/1.1\r\n";
    msg.headers = headers_2;
    msg.length_headers = strlen(msg.headers);

    // Try parsing the request
    char key[] = "data";
    float value;
    err = rpc_field_float(&msg, key, &value, 20);
    if (err == -1) {
        return err;
    }

    // Make sure that the message was parsed correctly
    RPC_TEST_FLOAT_EQ(value, (float)4.2);

    // So that we dont try to free the headers
    msg.headers = NULL;

    // Free the message
    rpc_message_free(&msg);

    return EXIT_SUCCESS;
}

