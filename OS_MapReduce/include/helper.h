#ifndef HELPER_H
#define HELPER_H

#ifndef MAX_PATH
#define MAX_PATH 255
#endif

#ifndef MAP_STR_LEN
#define MAP_STR_LEN 14
#endif

#ifndef ALPH_SIZE
#define ALPH_SIZE 26
#endif

#ifndef LINE_SIZE
#define LINE_SIZE 64
#endif

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

void getMapperPath(char *mapper_path, char *input_dir, int mapper);
pid_t r_wait(int *stat_loc);

#endif //HELPER_H
