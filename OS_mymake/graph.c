/* Dependency for main.c - functions to build/operate on targets */
#include "graph.h"

/* Creates and initializes a new target object, called in build_t_array of main.c */
target * init_target(char * t_name, unsigned char d_count, char ** t_deps){
    if(t_name == NULL || d_count < 0 || t_deps == NULL){
        printf("Invalid input to init_target\n");
        return NULL;
    }

    target * t;
    if( (t = (target *)malloc(sizeof(target))) == NULL ){
        printf("Failed to malloc target * in init_target.\n");
        return NULL;
    }

    t->name = t_name;
    t->dep_count = d_count;
    t->recipe_count = 0;
    t->visited = 0;     //changed only when popped from stack structure
    
    //adds dependencies to target object
    for(int i = 0; i < d_count; i ++)
        *((t->depend) + i) = t_deps[i];
    
    return t;
}

/* prints a target's members, called in print_t_array in main.c */
void print_target(target * t){
    if(t == NULL){
        printf("Null target\n");
        return;
    }
    printf("Target '%s' has %d dependencies and %d recipe(s)\n"
            , t->name, t->dep_count, t->recipe_count);
    
    //print depencies
    for(int i = 0; i < t->dep_count; i++)
        printf("Dependency %d is %s\n", i, *(t->depend + i));
    //print recipes
    for(int i = 0; i < t->recipe_count; i++)
        printf("Recipe %d is %s\n", i, *(t->recipe + i));
}

/* add recipe to target object, called in build_t_array in main.c */
int add_rcp(target *t, char * r){
    if(t == NULL || r == NULL){
        printf("Invalid input to add_rcp\n");
        return -1;
    }
    unsigned char count = t -> recipe_count;
    *((t->recipe) + count) = r;
    t->recipe_count = count + 1;
    return 0;
}

/*
for DFS stack traversal, marks a target as visited.
called in print_rcp_order and execute_stack in main.c
*/
int been_visited(target * t){
    if(t == NULL){
        printf("Incorrect param to been_visited.\n");
        return -1;
    }
    t->visited = 1;
    return 0;
}