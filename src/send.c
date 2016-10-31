#include <rpc.h>

// Parse the mesage
int rpc_message_send(struct rpc_message * msg) {
    int err;

    // Check if we already know what kind of message this is
    if (msg->protocol == RPC_PROTOCOL_UNKNOWN) {
        // Needs to be a valid protocol
        return -1;
    }

    switch (msg->protocol) {
    case RPC_PROTOCOL_RAW:
        return rpc_message_send_raw(msg);
    }

    errno = ENOPROTOOPT;
    return -1;
}
