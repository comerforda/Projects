#ifndef __DFS_STACK_H
#define __DFS_STACK_H
#define MAX_STACK MAX_TARGETS*MAX_DEP

#include <stdlib.h>
#include <stdio.h>
#include "graph.h"

/* implementation of a stack for DFS traversal */
typedef struct t_stack{
    int top;
    target* starray[MAX_STACK];
} t_stack;

t_stack* init_stack(){
    t_stack* stack = (t_stack*)malloc(sizeof(t_stack));
    stack->top = -1;
    return stack;
}

/*Adds target t to the the top of the t_stack stack*/
void push(t_stack* stack, target* t){
    if (stack->top == MAX_STACK - 1)
        return;
    stack->starray[++stack->top] = t;
}

/*Removes target at the top of the t_stack stack and returns a pointer to it*/
target* pop(t_stack* stack){
    if (stack->top == -1)
        return NULL;
    return stack->starray[stack->top--];
}

#endif