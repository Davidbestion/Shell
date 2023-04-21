#include <unistd.h>
#include "shell.h"
#include "parser.h"
#include "scanner.h"
#include "node.h"
#include "source.h"

struct node_s *parse_simple_command(struct token_s *tok)
{
    // If the input token is NULL, return NULL
    if(!tok)
    {
        return NULL;
    }
     // Create a new node of type NODE_COMMAND
    struct node_s *cmd = new_node(NODE_COMMAND);
     // If memory allocation fails, free input token and return NULL
    if(!cmd)
    {
        free_token(tok);
        return NULL;
    }
     // Get the source of the input token
    struct source_s *src = tok->src;
     // Loop until end of file token is reached
    do
    {
        // If the first character of the input token is a newline character,
        // free the token and break out of the loop
        if(tok->text[0] == '\n')
        {
            free_token(tok);
            break;
        }        
         // Create a new node of type NODE_VAR
        struct node_s *word = new_node(NODE_VAR);
         // If memory allocation fails, free nodes and input token, and return NULL
        if(!word)
        {
            free_node_tree(cmd);
            free_token(tok);
            return NULL;
        }
         // Set the text value of the input token to the string value of the new node.
        set_node_val_str(word, tok->text);
         // Add the word node as a child of the cmd node
        add_child_node(cmd, word);
         // Free the input token
        free_token(tok);    
    } while((tok = tokenize(src)) != &eof_token);    // Get the next token
     // Return the cmd node
    return cmd;
}