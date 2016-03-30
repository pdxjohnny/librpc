/*
 * Functions in librpc
 * librpc provides a client and server class
 *
 * Server
 *  Needs methods
 *      MethodName: MethodFunction
 *  Needs names of functions and pointers to those functions so it knows which
 *  function to call
 *
 *  Needs stuct of data that will be sent to method
*/

// So that cpp can link to this lib
#if defined (__cplusplus)
extern "C" {
#endif

// rpc_test should return 42
int rpc_test();

// The message sent or received
struct rpc_message {
    unsigned int length;
    char ** headers;
    char * body;
    char * raw;
};

// Fills field with the data sent over the request
// return code is 0 for error 1 for success
int rpc_field(char * field, char * ret, struct rpc_message * msg);
int rpc_int_field(int *);
int rpc_float_field(float *);
int rpc_char_field(char *);

// Server handler
struct rpc_handler {
    char * URL;
    int (*handler)(struct rpc_message *);
};

// Takes a NULL terminated array of rpc_handlers
int rpc_start_server(struct rpc_handler *);

// So that cpp can link to this lib
#if defined (__cplusplus)
}
#endif

// Error codes
// ENOSOCK could not create server socket
#define ENOSOCK 1
// ENOBIND means that we coundnt bind the socket
#define ENOBIND 2
// ELISTEN means that there was an error listening
#define ELISTEN 3
// EACCEPT means that there was an error accepting the connection
#define EACCEPT 4
