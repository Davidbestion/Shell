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
extern char **__environ;
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
        else    //file not found 
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
        execv(argv[0], argv);//Executes the command in argv[0] with 
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
    struct node_s *sibling = child->next_sibling;
    char *dir;
    if(!sibling){dir = NULL;}
    else {dir = sibling->val.str;}
    if(strcmp(child->val.str, "cd") == 0)
    {
        return execute_cd(dir);
    }
    free(sibling);
    if(find_pipe_command(node))//If it is a pipe command
    {
        printf("SALIO DEL find_pipe_command");
        return 1;//return 1 because it has been already processed
    }
    find_exit_command(node);
    if ((node->children <= 2) && (child->type == NODE_COMMAND) && (strcmp(child->val.str, "cd") == 0))
    {
        child = child->next_sibling;
        return execute_cd(child->val.str);
    }
    int argc = 0;
    long max_args = 255; // Maximum number of arguments that can be passed to a command
    char *argv[max_args+1]; // Keep 1 for the terminating NULL arg
    char *str;

    // Extracts the command's arguments.
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
    // Forks a child process, and executes the command in the child process using `do_exec_cmd()`.
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
     // Waits for the child process to finish and frees the memory allocated for the argument list.
    int status = 0;
    waitpid(child_pid, &status, 0);
    free_argv(argc, argv);
    return 1;
}

// Function that executes the "cd" commant.
int execute_cd(char* directory) {
    char path[1024];
    if (directory == NULL) {
        const char* home_dir = getenv("HOME");
        printf("LA DIRECCION DE HOME ES %s\n", home_dir);
        if (home_dir == NULL) {
            errno = ENOENT;
            fprintf(stderr, "cd: HOME environment variable not set.\n");
            return 0;
        }
        directory = (char*)home_dir;
    }
    if (chdir(directory) != 0) {
        fprintf(stderr,"cd: %s: %s\n", directory, strerror(errno));
        return 0;
    }
    if (getcwd(path, 1024) == NULL) {
        fprintf(stderr, "cd: getcwd failed\n");
        return 0;
    }
    if (setenv("PWD", path, 1) != 0) {
        fprintf(stderr,"cd: setenv failed\n");
        return 0;
    }
    return 1;
}

//Function that searches for the "|" command and executes it.
int find_pipe_command(struct node_s *node)
{
    struct node_s *curchild = node->first_child;//current child
    while(curchild->next_sibling)
    {
        curchild = curchild->next_sibling;//Sets curchild as the last child of node.
    }
    while(curchild->prev_sibling)//Searches by all the siblings
    {
        if(strcmp(curchild->val.str, "|") == 0)//If it found the command
        {
            struct node_s *node2 = new_node(NODE_COMMAND);//Creates a second AST.
            add_child_node(node2, curchild->next_sibling);

            //Erase the conections between curchild and his siblings and vice versa.
            node2->first_child->prev_sibling = NULL;
            curchild->next_sibling = NULL;
            curchild->prev_sibling->next_sibling = NULL;
            curchild->prev_sibling = NULL;

            return execute_pipe(node, node2);//Creates the pipe and executes the commands.
        }
        else//If not
        {
            curchild = curchild->prev_sibling;//Continue searching.
        }
    }
    return 0;
}
int execute_pipe(struct node_s *node1, struct node_s *node2)
{
    int fd[2]; // array to hold file descriptors for pipe
    if (pipe(fd) == -1) { // create pipe and check for errors
        printf("pipe failed\n"); // print error message if pipe fails
        return 0;
    }
    pid_t pid = fork(); // fork first child process
    if (pid == -1) {
        printf("fork failed\n"); // print error message if fork fails
        return 0;
    }
    if (pid == 0) { // child process 1
        dup2(fd[1], STDOUT_FILENO); // redirect output to write end of pipe
        //close(fd[0]); // close read end of pipe
        close(fd[1]); // close write end of pipe
        do_simple_command(node1);
    }
    else { // parent process
        waitpid(pid, NULL, 0); // wait for child process 1 to finish
        dup2(fd[0], STDIN_FILENO); // redirect input from read end of pipe
        close(fd[0]); // close read end of pipe
        //close(fd[1]); // close write end of pipe
        do_simple_command(node2);
        printf("execvp failed\n"); // print error message if execvp fails
    }
    return 1;
}

//Function that executes the "exit" command. It simply exits the process if findes the command.
void find_exit_command(struct node_s *node)
{
    if( node->val.str != NULL && strcmp(node->val.str, "exit") == 0) {
        exit(0);
    }        
}