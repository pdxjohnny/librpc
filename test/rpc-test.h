#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#include <librpc.h>

// Which tests to run
#define RPC_TEST_SERVER
#define RPC_TEST_CLIENT
#define RPC_TEST_MESSAGE

#define RPC_TEST(test_name) \
({\
    err = test_name();\
    if (err == -1) {\
        printf(#test_name " exited with status %d\n", errno);\
    }\
})

#define RPC_TEST_EQ(var, should_be) \
({\
    if (var != should_be) {\
        printf(#var " should have been " #should_be " but was \"%x\"\n", var);\
    }\
})

#define RPC_TEST_FLOAT_EQ(var, should_be) \
({\
    if (var != should_be) {\
        printf(#var " should have been " #should_be " but was \"%lf\"\n", var);\
    }\
})

#define RPC_TEST_STR_EQ(var, should_be) \
({\
    if (0 != strcmp(var, should_be)) {\
        printf(#var " should have been " #should_be " but was \"%s\"\n", var);\
    }\
})


// Server Tests
int test_server_correct_port();

// Client Tests
int test_client_contact_server();

// Message Tests
int test_message_parse();
int test_message_malloc_free_buffer();
int test_rpc_message_parse_http_path();
int test_rpc_message_parse_http_header();
int test_rpc_message_parse_http_data();
int test_rpc_message_parse_http_headers_too_long();
int test_rpc_message_parse_http_data_path();
int test_rpc_field();
int test_rpc_field_int();
int test_rpc_field_float();

