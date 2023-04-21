#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "shell.h"
#include "source.h"
#include "parser.h"
#include "executor.h"

// This program reads user input from standard input and echoes it back to the console
int main(int argc, char **argv) {
    char *cmd; // Pointer to dynamically allocated memory to store user input
     // Repeatedly prompt the user for input until the user enters the "exit" command
    do {
        // Print the primary prompt
        print_prompt1(); 
         // Read user input from standard input
        cmd = read_cmd();
         // If reading user input failed, exit the program with a successful exit status
        if (!cmd) {
            exit(EXIT_SUCCESS);
        }
         // If user entered an empty string or a newline, free memory and continue to next iteration of the loop
        if (cmd[0] == '\0' || strcmp(cmd, "\n") == 0) {
            free(cmd);
            continue;
        }
         // If user entered the "exit" command, free memory and exit the loop
        if (strcmp(cmd, "exit\n") == 0) {
            free(cmd);
            break;
        }
         // If user entered valid input, process it.
        struct source_s src;
        src.buffer   = cmd;
        src.bufsize  = strlen(cmd);
        src.curpos   = INIT_SRC_POS;
        parse_and_execute(&src);        
        
        free(cmd);    
    } while (1);    

    exit(EXIT_SUCCESS);
}

// This function reads a user command from standard input and returns a pointer to a buffer containing the command
char *read_cmd(void)
{
    char buf[1024]; // Buffer to store input read from standard input
    char *ptr = NULL; // Pointer to dynamically allocated memory to store the command
    char ptrlen = 0; // Length of the command in bytes
     // Continuously read input from standard input until the user presses the Enter key
    while(fgets(buf, 1024, stdin))
    {
        int buflen = strlen(buf); // Length of the input read from standard input
         // Allocate memory for the command if it hasn't been allocated yet
        if(!ptr)
        {
            ptr = malloc(buflen+1);
        }
        // Reallocate memory for the command if it has already been allocated
        else
        {
            char *ptr2 = realloc(ptr, ptrlen+buflen+1);
            if(ptr2)
            {
                ptr = ptr2;
            }
            else
            {
                free(ptr);
                ptr = NULL;
            }
        }
         // If memory allocation fails, return NULL
        if(!ptr)
        {
            fprintf(stderr, "error: failed to alloc buffer: %s\n", 
                    strerror(errno));
            return NULL;
        }
         // Copy the input read from standard input to the dynamically allocated buffer
        strcpy(ptr+ptrlen, buf);
         // If the last character of the input read from standard input is a newline character
        if(buf[buflen-1] == '\n')
        {
            // If the input read is a single newline character, return the command
            if(buflen == 1 || buf[buflen-2] != '\\')
            {
                return ptr;
            }
             // If the input read spans multiple lines, remove the backslash character and print a secondary prompt
            ptr[ptrlen+buflen-2] = '\0';
            buflen -= 2;
            print_prompt2();
        }
         // Update the length of the command in bytes
        ptrlen += buflen;
    }
     // Return the command
    return ptr;
}

int parse_and_execute(struct source_s *src)
{
    // Takes out white spaces and separates the input in tokens.
    skip_white_spaces(src);    
    struct token_s *tok = tokenize(src);    
    
    if(tok == &eof_token)
    {
        return 0;
    }    
    // Process the command in input.
    while(tok && tok != &eof_token)
    {
        struct node_s *cmd = parse_simple_command(tok);        
        
        if(!cmd)
        {
            break;
        }        
        do_simple_command(cmd);
        free_node_tree(cmd);
        tok = tokenize(src);
    }    
    
    return 1;
}