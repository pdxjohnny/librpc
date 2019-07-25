#include <rpc.h>

// int rpc_message_parse_http(struct rpc_message * msg, const char * buffer, int buffer_size) {

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
	struct rpc_message msg;

	rpc_message_parse_http(&msg, (const char *)Data, Size);

  return 0;
}
