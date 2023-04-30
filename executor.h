#ifndef BACKEND_H
#define BACKEND_H
#include "node.h"

char *search_path(char *file);
int do_exec_cmd(int argc, char **argv);
int do_simple_command(struct node_s *node);

int execute_cd(char* directory);
int find_pipe_command(struct node_s *node);
int execute_pipe(struct node_s *node1, struct node_s *node2);

void find_exit_command(struct node_s *node);

#endif