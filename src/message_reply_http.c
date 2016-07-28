#include <rpc.h>

int rpc_message_reply_http_413(struct rpc_message * msg) {
    msg->incomplete = 1;
    write(msg->client, RPC_REPLY_HTTP_413, strlen(RPC_REPLY_HTTP_413));
    return EXIT_SUCCESS;
}

int rpc_message_reply_http_404(struct rpc_message * msg) {
    msg->incomplete = 1;
    write(msg->client, RPC_REPLY_HTTP_404, strlen(RPC_REPLY_HTTP_404));
    return EXIT_SUCCESS;
}
