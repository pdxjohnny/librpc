#include <rpc.h>

// Parse an raw mesage
int rpc_message_parse_raw(struct rpc_message * msg, const char * buffer, int buffer_size) {
    // Just put everything we got in the buffer
    return rpc_message_append_to_buffer(msg, buffer, buffer_size);
}
