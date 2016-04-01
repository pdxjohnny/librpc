#include "rpc.h"
#include <librpc.h>

// Fills first with the first section of src, segment is up to delim
int rpc_string_untildelim(char * dest, char * src, int dest_size, char delim) {
    // Pointer the the first delim in src
    char * first_delim = NULL;
    // Check the first segment of the message
    first_delim = strchr(src, delim);
    // first_delim will be NULL if it couldnt find a delim in the src
    if (first_delim == NULL) {
        errno = EBADMSG;
        return -1;
    }
    // Find out how many characters in between the begining and the delim
    uintptr_t first_segment_length = first_delim - src;
    // We only have a dest of dest_size and we have to NULL terminate dest so
    // it can only by dest_size - 1 long
    if (first_segment_length > (dest_size - 1)) {
        errno = ERANGE;
        return -1;
    }
    // Copy the first segment from src into its own string
    memcpy(dest, src, first_segment_length);
    // NULL terminate the first segment string
    dest[first_segment_length] = '\0';

    return EXIT_SUCCESS;
}

