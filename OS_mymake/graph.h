/* target struct declaration and function headers for graph.c*/

#ifndef __GRAPH_H
#define __GRAPH_H

#define LINE_SIZE 128
#define MAX_LINES 128
#define MAX_DEP 8
#define MAX_RECIPES_PT 8
#define MAX_TARGETS 128
#define MAX_PARM 32

#include <stdio.h>
#include <stdlib.h>

typedef struct target_block {
	char *name;
	char *depend[MAX_DEP]; 
	char *recipe[MAX_RECIPES_PT];
	unsigned char dep_count;
	unsigned char recipe_count;
	unsigned char visited;
} target;

char lines[MAX_LINES][LINE_SIZE]; //for input parsing, initialized in process_file in helper.c

//function declarations for graph.c
target * init_target(char * t_name, unsigned char d_count, char ** t_deps);
void print_target(target *t); 
int add_rcp(target *t, char * r);
int been_visited(target * t);

#endif
