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

// Take a NULL terminated string allocated on the stack and allocate the amount
// needed to hold the string on the heap and copy it over to the heap. But
// never allocate more than max
char * rpc_string_on_heap(char * src, size_t max) {
    // The length of the string to be allocated
    // Add one so that we have room for the NULL terminator
    // Subtract one from max because we have to add one on the case that it
    // doesnt stop at max, then it needs to have one more byte for the NULL
    // terminator
    size_t length = strnlen(src, max - 1) + 1;

    // Allocate the string on the heap
    char * heap_string = malloc(length);
    // If memory could not be allocated return NULL
    if (heap_string == NULL) {
        return NULL;
    }
    // Copy the contents over
    strncpy(heap_string, src, length);
    return heap_string;
}

