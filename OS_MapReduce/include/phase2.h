#ifndef PHASE2_H
#define PHASE2_H
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tuple.h"

// Function prototypes for
//map function that will go through text files in the Mapper_i.txt to get the (letter, wordcount)
int setUpMappers(int *pipes, int m, char *input_dir);
int processText(char *mapper_path, tuple *letters[]);
int setMapperPipe(int *pipes, int m, int mapper);
int closeWritingPipes(int *pipes, int m);

#endif // PHASE2_H 