#include "rpc.h"
#include <librpc.h>

int rpc_test () {
    return 42;
}

// Search the response for field and store it in ret
int rpc_field(char * field, char * ret, struct rpc_message * msg) {
    return 0;
};

