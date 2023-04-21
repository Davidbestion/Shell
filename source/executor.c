#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "shell.h"
#include "node.h"
#include "executor.h"
 // Searches for the given file in the system's PATH directories and returns the
// full path of the executable if found. Otherwise, returns NULL.
char *search_path(char *file)
{
    char *PATH = getenv("PATH");
    char *p    = PATH;
    char *p2;
     while(p && *p)
    {
        p2 = p;
        while(*p2 && *p2 != ':')
        {
            p2++;
        }
         int  plen = p2-p;
        if(!plen)
        {
            plen = 1;
        }
         int  alen = strlen(file);
        char path[plen+1+alen+1];
         strncpy(path, p, p2-p);
        path[p2-p] = '\0';
         if(p2[-1] != '/')
        {
            strcat(path, "/");
        }
        strcat(path, file);
         struct stat st;
        if(stat(path, &st) == 0)
        {
            if(!S_ISREG(st.st_mode))
            {
                errno = ENOENT;
                p = p2;
                if(*p2 == ':')
                {
                    p++;
                }
                continue;
            }
            p = malloc(strlen(path)+1);
            if(!p)
            {
                return NULL;
            }
             strcpy(p, path);
            return p;
        }
        else    /* file not found */
        {
            p = p2;
            if(*p2 == ':')
            {
                p++;
            }
        }
    }
    errno = ENOENT;
    return NULL;
}
 // Executes the given command with the specified arguments (argc, argv). If the
// command does not contain a '/', searches for the executable in the system's
// PATH using the `search_path()` function.
int do_exec_cmd(int argc, char **argv)
{
    if(strchr(argv[0], '/'))
    {
        execv(argv[0], argv);
    }
    else
    {
        char *path = search_path(argv[0]);
        if(!path)
        {
            return 0;
        }
        execv(path, argv);
        free(path);
    }
    return 0;
}
 // Frees the memory allocated for the argument list (argv).
static inline void free_argv(int argc, char **argv)
{
    if(!argc)
    {
        return;
    }
    while(argc--)
    {
        free(argv[argc]);
    }
}
 // Processes a simple command represented by a node in an abstract syntax tree.
// Extracts the command's arguments, forks a child process, and executes the
// command in the child process using `do_exec_cmd()`. Waits for the child
// process to finish and frees the memory allocated for the argument list.
int do_simple_command(struct node_s *node)
{
    if(!node)
    {
        return 0;
    }
    struct node_s *child = node->first_child;
    if(!child)
    {
        return 0;
    }
     int argc = 0;
    long max_args = 255; // Maximum number of arguments that can be passed to a command
    char *argv[max_args+1]; // Keep 1 for the terminating NULL arg
    char *str;
     while(child)
    {
        str = child->val.str;
        argv[argc] = malloc(strlen(str)+1);
         if(!argv[argc])
        {
            free_argv(argc, argv);
            return 0;
        }
         strcpy(argv[argc], str);
        if(++argc >= max_args)
        {
            break;
        }
        child = child->next_sibling;
    }
    argv[argc] = NULL;
     pid_t child_pid = 0;
    if((child_pid = fork()) == 0)
    {
        // Child process
        do_exec_cmd(argc, argv);
        fprintf(stderr, "error: failed to execute command: %s\n",
                strerror(errno));
        if(errno == ENOEXEC)
        {
            exit(126);
        }
        else if(errno == ENOENT)
        {
            exit(127);
        }
        else
        {
            exit(EXIT_FAILURE);
        }
    }
    else if(child_pid < 0)
    {
        // Error: Failed to fork child process
        fprintf(stderr, "error: failed to fork command: %s\n",
                strerror(errno));
        return 0;
    }
     // Parent process
    int status = 0;
    waitpid(child_pid, &status, 0);
    free_argv(argc, argv);
     return 1;
}