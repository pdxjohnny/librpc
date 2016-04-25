#include "rpc-test.h"

int main (int argc, char ** argv, char ** env) {
    int err;

    // Run the test function
    RPC_TEST_EQ(rpc_test(), 42);

    // Run all the tests

#ifdef RPC_TEST_SERVER
    // Server Tests
    RPC_TEST(test_server_correct_port);
#endif

#ifdef RPC_TEST_CLIENT
    // Client Tests
    RPC_TEST(test_client_contact_server);
#endif

#ifdef RPC_TEST_MESSAGE
    // Message Tests
    RPC_TEST(test_message_parse);
    RPC_TEST(test_message_malloc_free_buffer);
    RPC_TEST(test_rpc_message_parse_http_path);
    RPC_TEST(test_rpc_message_parse_http_header);
    RPC_TEST(test_rpc_message_parse_http_data);
    RPC_TEST(test_rpc_message_parse_http_headers_too_long);
    RPC_TEST(test_rpc_message_parse_http_data_path);
    RPC_TEST(test_rpc_field);
    RPC_TEST(test_rpc_field_int);
    RPC_TEST(test_rpc_field_float);
#endif

    return EXIT_SUCCESS;
}

