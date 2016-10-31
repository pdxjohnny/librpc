#include <rpc.h>

int main (int argc, char **argv) {
    int err;
    uint16_t port = 0;
    char *msg_buffer;

    if (argc < 3) {
        printf("Usage: %s port message\n", argv[0]);
        return EXIT_FAILURE;
    }

    port = strtoul(argv[1], NULL, 10);
    msg_buffer = argv[2];

    // Configure the client
    char addr[] = "127.0.0.1";
    struct rpc_client_config client_config = {
       /* .addr = */ addr,
       /* .port = */ port,
       /* .client = */ -1,
    };

    // Start the client
    err = rpc_client_start(&client_config);
    if (err != EXIT_SUCCESS) {
        printf("There was an error starting the client %d\n", err);
        return EXIT_FAILURE;
    }

    // Create the message
    struct rpc_message msg;
    rpc_message_init(&msg);
    // Use the client we have setup
    msg.client = client_config.client;
    // Fill all the fields
    msg.method = NULL;
    msg.headers = NULL;
    msg.buffer = msg_buffer;
    msg.length = strlen(msg_buffer);
    // Specify the protocol we want to send as
    msg.protocol = RPC_PROTOCOL_RAW;

    // Send the message
    err = rpc_message_send(&msg);
    if (err != EXIT_SUCCESS) {
        printf("There was an error sending the message %d\n", err);
        return EXIT_FAILURE;
    }

    // Get the reply
    struct rpc_message reply;
    rpc_message_init(&reply);
    // Use the client we have setup
    reply.client = client_config.client;
    // Specify the protocol we want to recv as (because its impossible to auto
    // determin with RAW
    reply.protocol = RPC_PROTOCOL_RAW;
    // Get the rpc servers reply to our message
    err = rpc_message_recv(&reply);
    if (err != EXIT_SUCCESS) {
        printf("There was an error receiving the message %d\n", err);
        return EXIT_FAILURE;
    }

    // Print the reply
    printf("%s", reply.buffer);

    // Free the reply
    rpc_message_free(&reply);

    return EXIT_SUCCESS;
}
