#ifndef PHASE4_H
#define PHASE4_H
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "helper.h"

int produceFinalOutput(char *reducer_path, char *output_path);

#endif /* PHASE4_H */