#ifndef PHASE1_H
#define PHASE1_H
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "helper.h"

int makeMapperInput(char *path, char *input_dir, int m);
int traverse(char *path, char *input_dir, int m, int mapper, int *total);
int nextMapper(int mapper, int m);

#endif /* PHASE1_H */