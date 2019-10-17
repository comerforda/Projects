#include <errno.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "graph.h"

int process_file(char *fname);
int makeargv(const char *s, const char *delimiters, char ***argvp);
pid_t r_wait(int *stat_loc);