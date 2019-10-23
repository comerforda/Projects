// Add Guard to header file
// Function prototype for
//			reduce function that will receive list from all the Mappers to create a single list
//			the single list is then written to ReducerResult.txt
#ifndef PHASE3_H
#define PHASE3_H
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tuple.h"

int reduce(int *pipes, int m, char *reducer_path);
int closeReadingPipes(int *pipes, int m);

#endif /* PHASE3_H */