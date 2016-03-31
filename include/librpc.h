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
#include <stdint.h>
#include <errno.h>

// So that cpp can link to this lib
#if defined (__cplusplus)
extern "C" {
#endif

// rpc_test should return 42
int rpc_test();

// The message sent or received
struct rpc_message {
    // Length of the message
    unsigned int length;
    // Length recieved
    unsigned int length_recv;
    // Boolean values
    // Parse comepleted
    char parse_complete;
    // Parse was not comepleted
    char incomplete;
    // Headers
    char ** headers;
    // Data passed
    char ** data;
    char * body;
    char * raw;
};

// Initializes an rpc_message
int rpc_message_init(struct rpc_message * msg);

// Parse the message into the message struct, give it the buffer which contains
// new data to be parsed and the number of bytes in the buffer
int rpc_message_parse(struct rpc_message *, char *, int);

// Free the message when we are done with it
int rpc_message_free(struct rpc_message *);

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

// Server config
struct rpc_server_config {
    // Address to bind to
    char * addr;
    // Port ot bind to (if set to zero it will be set by rpc_start_server once
    // bound
    uint16_t port;
    // The NULL terminated array of handlers for each path
    struct rpc_handler * handlers;
    // Used to stop the server gracefully. This is the file descriptor of
    // anything that can be feed to select. See man 3 pselect for details
    // It is an array which needs the first fd to be readable and the second to
    // be writeable (a pipe works great)
    int * comm;
};

// Start the server and listen for client requests
int rpc_server_start(struct rpc_server_config *);

// Start the server in the background
int rpc_server_start_background(struct rpc_server_config * config);

// Stop the server gracefully
int rpc_server_stop(struct rpc_server_config * config);

// Handle incomming client connections
int rpc_server_handle_client(struct rpc_server_config * config, struct sockaddr_in client_addr, int client);

// Client config
struct rpc_client_config {
    // The address of the server we are connecting to
    char * addr;
    // The port of the server we are connecting to
    uint16_t port;
    // The message to deliver
    struct rpc_message * msg;
};

// Client connectes to the server and preforms requests
int rpc_client(struct rpc_client_config * config);


// So that cpp can link to this lib
#if defined (__cplusplus)
}
#endif

// Constants
// Ends of a pipe
#define RPC_COMM_READ 0
#define RPC_COMM_WRITE 1
// Port length shouldnt be longer than 12
#define RPC_GET_PORT_BUFFER_SIZE 12
// For reading in client messages
#define RPC_MESSAGE_BUFFER_SIZE 100

// Error codes
// ENOSOCK could not create server socket
#define ENOSOCK 1
// ENOBIND means that we coundnt bind the socket
#define ENOBIND 2
// ELISTEN means that there was an error listening
#define ELISTEN 3
// EACCEPT means that there was an error accepting the connection
#define EACCEPT 4
// ESELECT means that there was an error with select when trying to determin if
// there is a new connection to accept
#define ESELECT 5
// ENOCONNECT means that the client could not connect to the server
#define ENOCONNECT 6
// EPORT is when the server trys to find out what port it is bound to but it
// fails
#define EPORT 7
// EADDR is when the client cant look up the ip of the hostname given
#define EADDR 8
// EBACKGROUND means we could not fork/thread into the background
#define EBACKGROUND 9
// ECREATECOMM is when we cant establish a communitcation to the server process
#define ECREATECOMM 10
