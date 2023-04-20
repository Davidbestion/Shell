#include <errno.h>  
#include "shell.h"  // Header file for shell program.
#include "source.h"  // Header file for source functions.

void unget_char(struct source_s *src) {
    if (src->curpos < 0) {  
        return;  
    }    
    src->curpos--;   
}

char next_char(struct source_s *src) {
    if (!src || !src->buffer) {  // Check if either src or src's buffer is null.
        errno = ENODATA;  // If either is null, set error number to ENODATA (no data available).
        return ERRCHAR;  // Return ERRCHAR, which is defined in the source.h header file.
    }   

    char c1 = 0;  
    if (src->curpos == INIT_SRC_POS) {   // Check if the current position is equal to the initial position.
        src->curpos  = -1;  // If it is, set it to -1.
    }
    else {   // If it's not equal to the initial position,
        c1 = src->buffer[src->curpos];  // Set c1 to the character at the current position in the buffer.
    } 

    if (++src->curpos >= src->bufsize) {  // Increment the current position and check if it's greater than or equal to the buffer size.
        src->curpos = src->bufsize;  // If it is, set the current position to the buffer size.
        return EOF;  // Return EOF (end of file), which is defined in stdio.h library.
    }   

    return src->buffer[src->curpos]; // Return character at the current position in the buffer.
}

char peek_char(struct source_s *src) {
    if (!src || !src->buffer) {  // Check if either src or src's buffer is null.
        errno = ENODATA;  // If either is null, set error number to ENODATA (no data available).
        return ERRCHAR;  // Return ERRCHAR, which is defined in the source.h header file.
    }  

    long pos = src->curpos;  
    
    if (pos == INIT_SRC_POS) {  // Check if the pos is equal to the initial source position.
        pos++;  // If it is, increment it.
    }
    pos++;  // Increment pos.
    
    if (pos >= src->bufsize) {  // Check if pos is greater than or equal to the buffer size.
        return EOF;  // Return end of file.
    }   

    return src->buffer[pos];  // Return the character at position pos from the buffer.
}

void skip_white_spaces(struct source_s *src) {
    char c;    
    if (!src || !src->buffer) {  // Check if either src or src's buffer is null.
        return;  // If either is null, return control to the calling function.
    }    
    
    while (((c = peek_char(src)) != EOF) && (c == ' ' || c == '\t')) {  // While there are characters to read and if they are white spaces,
        next_char(src);  // Read the next character.
    }
}