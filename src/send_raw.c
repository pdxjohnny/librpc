#include <rpc.h>

int rpc_message_send_raw(struct rpc_message * msg) {
    int len_sent = write(msg->client, msg->buffer, msg->length);

    // Make sure that the size sent was what we wanted to send
    if (len_sent != msg->length) {
        return -1;
    }

    return EXIT_SUCCESS;
}
