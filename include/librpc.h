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
    // Length of the headers
    unsigned int length_headers;
    // The number of times we have received new data
    int recv_count;
    // The client that we are reading from
    int client;
    // Boolean values
    // Parse comepleted
    char parse_complete;
    // Parse was not comepleted
    char incomplete;
    // The protocol that was used to parse and should be used to send back to
    // the client
    char protocol;
    // The method to call
    char * method;
    // Headers
    char * headers;
    // Data paressed
    char * data;
    char * buffer;
};

// Initializes an rpc_message
int rpc_message_init(struct rpc_message * msg);

// Parse the message into the message struct, give it the buffer which contains
// new data to be parsed and the number of bytes in the buffer
int rpc_message_parse(struct rpc_message *, char *, int);
// Pick the correct parser based on msg->protocol
int rpc_message_parse_protocol(struct rpc_message *, char *, int);
// Append new data to the messages interal buffer
int rpc_message_append_to_buffer(struct rpc_message *, char *, int);
// Parse various kinds of messages
int rpc_message_parse_urlencoded(struct rpc_message *, char *, int);
int rpc_message_parse_json(struct rpc_message *, char *, int);
// Methods to parse for various protocols
// HTTP
int rpc_message_parse_http(struct rpc_message *, char *, int);
int rpc_message_parse_http_path(struct rpc_message *);
int rpc_message_parse_http_headers(struct rpc_message *);
int rpc_message_parse_http_body(struct rpc_message *);

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
    // This is the method that you want this handler to repsond to
    char * method;
    // Handlers should accpet the client file descriptor and the message sent
    // by the client
    int (*func)(int, struct rpc_message *);
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
    // The handler to call if no handlers are found that match the requested
    // method
    int (*not_found)(int, struct rpc_message *);
    // Used to stop the server gracefully. This is the file descriptor of
    // anything that can be feed to select. See man 3 pselect for details
    // It is an array which needs the first fd to be readable and the second to
    // be writeable (a pipe works great)
    int * comm;
};

// Start the server and listen for client requests
int rpc_server_start(struct rpc_server_config *);

// Start the server in the background
int rpc_server_start_background(struct rpc_server_config *);

// Stop the server gracefully
int rpc_server_stop(struct rpc_server_config *);

// Handle incomming client connections
int rpc_server_handle_client(struct rpc_server_config *, struct sockaddr_in *, int);

// Pick the correct handler to reply to the client
int rpc_server_reply_client(struct rpc_server_config *, struct sockaddr_in *, int, struct rpc_message *);

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

// String functions
//  Used for string minipulation
//
// Grab from start to character delim from src and put it in dest
int rpc_string_untildelim(char * dest, char * src, int dest_size, char delim);
// Allocate a string on the heap an copy something into it
char * rpc_string_on_heap(char * src, size_t max);


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
#define RPC_MESSAGE_BUFFER_SIZE 1024

// Protocols
#define RPC_PROTOCOL_UNKNOWN 0
#define RPC_PROTOCOL_HTTP 'h'

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
