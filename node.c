#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "shell.h"
#include "node.h"
#include "parser.h"

// Create a new node with the specified type and return a pointer to it
struct node_s *new_node(enum node_type_e type)
{
    // Allocate memory for the new node
    struct node_s *node = malloc(sizeof(struct node_s));    
    // If the allocation fails, return NULL
    if(!node)
    {
        return NULL;
    }
    // Initialize the new node with default values
    memset(node, 0, sizeof(struct node_s));
    node->type = type;
    return node;
}
 // Add a child node to a parent node
void add_child_node(struct node_s *parent, struct node_s *child)
{
    // If either the parent or the child node is NULL, do nothing
    if(!parent || !child)
    {
        return;
    }
    // If the parent node has no other children, make the new child the first child
    if(!parent->first_child)
    {
        parent->first_child = child;
    }
    // Otherwise, find the last child and add the new child to the end
    else
    {
        struct node_s *sibling = parent->first_child;
        // Navigate to the end of the sibling list
        while(sibling->next_sibling)
        {
            sibling = sibling->next_sibling;
        }
        // Add the new child to the end of the sibling list
        sibling->next_sibling = child;
        child->prev_sibling = sibling;
    }
    // Update the parent's children count
    parent->children++;
}
 // Set the value of a node to a string
void set_node_val_str(struct node_s *node, char *val)
{
    // Set the value type to string
    node->val_type = VAL_STR;
    // If the value is NULL, set the value to NULL
    if(!val)
    {
        node->val.str = NULL;
    }
    // Otherwise, allocate memory for the value and copy the contents of the given string
    else
    {
        char *val2 = malloc(strlen(val)+1);
        // If the allocation fails, set the value to NULL
        if(!val2)
        {
            node->val.str = NULL;
        }
        // Otherwise, copy the value to the new memory location
        else
        {
            strcpy(val2, val);
            node->val.str = val2;
        }
    }
}
 // Free a node tree and all its child nodes
void free_node_tree(struct node_s *node)
{
    // If the node is NULL, do nothing
    if(!node)
    {
        return;
    }
    // Recursively free all child nodes
    struct node_s *child = node->first_child;
    while(child)
    {
        struct node_s *next = child->next_sibling;
        free_node_tree(child);
        child = next;
    }
    // If the node contains a string, free the memory associated with it
    if(node->val_type == VAL_STR)
    {
        if(node->val.str)
        {
            free(node->val.str);
        }
    }
    // Free the memory associated with the node itself
    free(node);
}