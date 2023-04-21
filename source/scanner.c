#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "shell.h"
#include "scanner.h"
#include "source.h"

// Tokenizer buffer and related variables
char *tok_buf = NULL;
int   tok_bufsize  = 0;
int   tok_bufindex = -1;

 // Special token to indicate the end of input
struct token_s eof_token = 
{
    .text_len = 0,
};

 // Function to add a character to the tokenizer buffer (tok_buf) and expand the buffer size if needed
void add_to_buf(char c)
{
    // Add the character to the token buffer
    tok_buf[tok_bufindex++] = c;    
     // Check if the buffer is full and needs to be expanded
    if(tok_bufindex >= tok_bufsize)
    {
        // Double the buffer size and reallocate memory
        char *tmp = realloc(tok_buf, tok_bufsize * 2);        
         // Check if memory allocation failed
        if(!tmp)
        {
            errno = ENOMEM;
            return;
        }        
        tok_buf = tmp;
        tok_bufsize *= 2;
    }
}

 // Function to create a token from a given string
struct token_s *create_token(char *str)
{
    // Allocate memory for the token structure
    struct token_s *tok = malloc(sizeof(struct token_s));
     // Check if memory allocation failed
    if(!tok)
    {
        return NULL;
    }    
     // Initialize the token structure with zero values
    memset(tok, 0, sizeof(struct token_s));
    // Set the text length for the token
    tok->text_len = strlen(str);
     // Allocate memory for the token text
    char *nstr = malloc(tok->text_len + 1);
     // Check if memory allocation failed
    if(!nstr)
    {
        free(tok);
        return NULL;
    }
     // Copy the input string to the token text
    strcpy(nstr, str);
    tok->text = nstr;
     return tok;
}
 // Function to free the memory allocated for a token
void free_token(struct token_s *tok)
{
    if(tok->text)
    {
        free(tok->text);
    }
    free(tok);
}
 // Main tokenizer function
struct token_s *tokenize(struct source_s *src)
{
    int endloop = 0;
     // Check for valid input
    if(!src || !src->buffer || !src->bufsize)
    {
        errno = ENODATA;
        return &eof_token;
    }
     // Initialize the tokenizer buffer if it is not already initialized
    if(!tok_buf)
    {
        tok_bufsize = 1024;
        tok_buf = malloc(tok_bufsize);
        if(!tok_buf)
        {
            errno = ENOMEM;
            return &eof_token;
        }
    }    
     // Reset the tokenizer buffer index
    tok_bufindex     = 0;
    tok_buf[0]       = '\0';
     // Read the next character from the input source
    char nc = next_char(src);    
     // Check if the character is an error or end-of-file (EOF)
    if(nc == ERRCHAR || nc == EOF)
    {
        return &eof_token;
    }    
     // Tokenization loop
    do
    {
        // Process the current character based on its type
        switch(nc)
        {
            case ' ':
            case '\t':
                // If the current character is a space or tab, and the token buffer is not empty,
                // mark the end of the token
                if(tok_bufindex > 0)
                {
                    endloop = 1;
                }
                break;
             case '\n':
                // If the current character is a newline, mark the end of the token
                if(tok_bufindex > 0)
                {
                    unget_char(src);
                }
                else
                {
                    add_to_buf(nc);
                }
                endloop = 1;
                break;
             default:
                // Add any other character to the token buffer
                add_to_buf(nc);
                break;
        }        
         // Check if the tokenization loop should stop
        if(endloop)
        {
            break;
        }    
    } while((nc = next_char(src)) != EOF);    
     // If the token buffer is empty, return the end-of-file token
    if(tok_bufindex == 0)
    {
        return &eof_token;
    }
     // Ensure the token buffer index is within bounds
    if(tok_bufindex >= tok_bufsize)
    {
        tok_bufindex--;
    }
     // Add a null terminator to the token buffer
    tok_buf[tok_bufindex] = '\0';
     // Create a token from the token buffer
    struct token_s *tok = create_token(tok_buf);
     // Handle memory allocation failure
    if(!tok)
    {
        fprintf(stderr, "error: failed to alloc buffer: %s\n", strerror(errno));
        return &eof_token;
    }     
     // Set the token's source to the input source
    tok->src = src;
     // Return the token
    return tok;
}