#include <stdio.h>
#include <unistd.h>
#include "shell.h"

void print_prompt1(void)
{
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    fprintf(stderr, "mi-shell: %s $ ", cwd);
}

void print_prompt2(void)
{
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    fprintf(stderr, "mi-shell: %s > ", cwd);
} 

