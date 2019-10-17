/*
Test machine: CSELAB kh4250 15
date: 09/30/2019
name: Azul Bolaños, Adam Comerford
x500: bolan108, comer059
*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "graph.h"
#include "dfs_stack.h"
#include "helper.h"

void print_ussage_message();
int build_t_array(target * t_array[], int lines_count);
void print_t_array(target * t_array[], int t_count);
int print_rcp_order(t_stack * stack);
int execute_stack(t_stack * stack);
int build_stack(t_stack * stack, target * t_array[], int array_size, target * head);
int index_of(target * t_array[], int array_size, char * t_name);

int main(int argc, char *argv[]){
    int target_count = 0;
    int lines_count;
    target * t_array[MAX_TARGETS];
    t_stack * stack = NULL;

    //check for improper input
    if(argc < 2 || argc > 3){
        print_ussage_message();
        return 1;
    }

	//input: mymake filename
    if(argc == 2){ 
        lines_count = process_file(argv[1]);                   //parse input
        if(lines_count == -1){   
            printf("Failed to process file %s\n",argv[1]);
            return 1;
        }
        target_count = build_t_array(t_array,lines_count);     //build target array
        //build stack starting with t_array[0]
        if(target_count > 0){
                stack = init_stack();
                build_stack(stack, t_array, target_count, t_array[0]);
        }
        execute_stack(stack);                                  //execute recipes

    }else{
        //input: mymake -p filename
        if(!strcmp(argv[1], "-p")){
            lines_count = process_file(argv[2]);               //parse input
            if(lines_count == -1){   
                printf("Failed to process file %s\n",argv[2]);
                return 1;
            }
            target_count = build_t_array(t_array,lines_count); //build target array
            print_t_array(t_array, target_count);              //print target array

        //input: mymake -r filename
        }else if(!strcmp(argv[1], "-r")){
            lines_count = process_file(argv[2]);               //parse input
            if(lines_count == -1){
                printf("Failed to process file %s\n",argv[2]);
                return 1;
            }
            target_count = build_t_array(t_array, lines_count);//build target array
            if(target_count > 0){
                stack = init_stack();                          //build target stack
                build_stack(stack, t_array, target_count, t_array[0]);
            }
            print_rcp_order(stack);                            //print instructions

        //input: mymake filename target
        }else{
            lines_count = process_file(argv[1]);               //parse input
            if(lines_count == -1){   
                printf("Failed to process file %s\n",argv[1]);
                return 1;
            }
            target_count = build_t_array(t_array, lines_count); //build target array

            //build stack starting with specified target
            if(target_count > 0){
                int index = index_of(t_array, target_count, argv[2]); //find idx of target
                if(index == -1)                                //target not found
                    printf("No target '%s' exists in %s\n", argv[2], argv[1]);
                else{
                    stack = init_stack();                      //create stack
                    build_stack(stack, t_array, target_count, t_array[index]);
                    execute_stack(stack);                      //execute recipes
                }
            }
        }
    }

    //freeing memory allocated
    if(stack != NULL)
        free(stack);
    if(target_count > 0){
        int i;
        for(i = 0; i < target_count; i++)
            free(t_array[i]);
    }
	exit(EXIT_SUCCESS);
}   //end main

/*
Creates an array of target objects and stores it in t_array. 
Targets are initialized based on the information of the lines array.
Returns count of targets added to t_array or -1 if an error was encountered.
*/
int build_t_array(target * t_array[], int lines_count){
    char tdelim[] = ": \t\n";   //delimitators for target lines
    char idelim[] = "\n\t";     //delimitators for instruction lines
    target * c_target;
    char  **tokens;
    int tokens_count;
    int target_count = 0;

    for(int i = 0; i < lines_count; i ++){
        if(strcmp(lines[i],"\n") != 0){       //ignore newlines

            if(strspn(lines[i],"\t") > 0){  
                //instruction code
                tokens_count = makeargv(lines[i], idelim, &tokens);    //tokenize input, eliminates delims
                if(tokens_count == -1){
                    printf("Failed to tokenize line in build_t_array\n");
                    return -1;
                }
                add_rcp(c_target, tokens[0] + strspn(tokens[0], " ")); //add instruction to target (see graph.c)
               
            }else{                          
                //target code
                tokens_count = makeargv(lines[i], tdelim, &tokens);    //tokenize input, eliminates delims
                if(tokens_count == -1){
                    printf("Failed to tokenize line in build_t_array\n");
                    return -1;
                }
                c_target = init_target(tokens[0], tokens_count - 1, (tokens + 1)); //initializes current target
                if(c_target == NULL){
                    printf("Failed to produce target in build_t_array\n");
                    return -1;
                }
                t_array[target_count++] = c_target;     //iterate through target array
            }
        }
    }
    return target_count;
}

/* 
Creates a stack of targets starting with the head target, and recursively adding on the 
head's dependencies and their corresponding dependencies. Stack allows BFS execution of the head target.
Function returns 0 if successful, else it returns -1.
*/
int build_stack(t_stack * stack, target * t_array[], int array_size, target * head){
    //check for erroneous array/stack conditions before building stack
    if(stack == NULL || head == NULL || t_array == NULL || array_size <= 0|| (index_of(t_array,array_size,head->name) == -1))
        return -1;
    target * t;
    int index;

    push(stack,head);
    //iterate through head's dependencies, recurse until we get down to base targets
    for(int i = head->dep_count - 1; i >= 0; i--){
        index = index_of(t_array, array_size, *(head->depend + i)); //check if head's depency is a target
        if(index != -1){
            t = t_array[index];
            if(t->dep_count > 0)
                build_stack(stack, t_array, array_size, t);
            else
                push(stack,t);      //push current target if no dependencies
        }
    }
    return 0;
}

/*
Prints all targets in the t_array as using the print_target function (see graph.c).
*/
void print_t_array(target * t_array[], int t_count){
    for(int i = 0; i < t_count; i++){
        print_target(t_array[i]);
        if(i < t_count - 1) printf("\n");
    }
}

/*
Returns the index of target with name t_name in the t_array. If t_array doesn't have a 
target of that name, then the program returns -1.
*/
int index_of(target * t_array[], int array_size, char * t_name){
    int index = -1;
    for(int i = 0; i < array_size; i ++){
        if( !(strcmp(t_array[i]->name,t_name)) )
            index = i;
    }
    return index;
}

/*
Traverses target stack t_stack and prints each target's instructions in order.
Returns zero if succeful, else it returns -1.
*/
int print_rcp_order(t_stack * stack){
    if(stack == NULL)
        return -1;

    target * t;
    int childpid, rcps;
    char **mul_rcps;        //for handling multiple instructions per target (EC)

    while(stack->top >= 0){
        t = pop(stack);
        //BFS traversal
        if(!(t->visited)){
            for(int i = 0; i < t->recipe_count; i++){
                rcps = makeargv(t->recipe[i], ",", &mul_rcps); //split up instructions if commas present (EC)
                if(rcps == -1){
                    printf("Failed to tokenize in execute_stack\n");
                    return -1;
                }
                for(int j = 0; j < rcps; j++){      //parallel execution (EC)
                    childpid = fork();
                    if(childpid < 0){
                        printf("Failed to create child to print stack.\n");
                        return -1;
                    }
                    //child code
                    if(childpid == 0){
                        printf("%s\n", mul_rcps[j] + strspn(mul_rcps[j], " ")); //print recipe child is executing
                        exit(0);         //kill child processes, prevent child from forking
                    }
                }
                while(r_wait(NULL) > 0); //have parent wait for all its children
            }
            been_visited(t);             //mark target as visited gets printed only once
        }
    }
    return 0;
}

/*
Traverses target stack t_stack, printing and executing each target's instructions in order. 
Returns zero if successful, else it returns -1.
*/
int execute_stack(t_stack * stack){
    if(stack == NULL)
        return -1;
    target * t;
    int childpid, rcps, tokens_count;
    char **mul_rcps;         //for handling comma-separated recipes (EC)
    char **tokens;

    while(stack->top >= 0){
        t = pop(stack);
        if(!(t->visited)){
            for(int i = 0; i < t->recipe_count; i++){ //traverse through the target's recipes
                rcps = makeargv(t->recipe[i], ",", &mul_rcps); //split up instructions if commas present (EC)
                if(rcps == -1){
                    printf("Failed to tokenize in execute_stack\n");
                    return -1;
                }
                for(int j = 0; j < rcps; j++){      //parallel execution (EC) 
                    childpid = fork();
                    if(childpid < 0){
                        printf("Failed to create child to exec stack.\n");
                        return -1;
                    }
                    //child code
                    if(childpid == 0){
                        printf("%s\n", mul_rcps[j] + strspn(mul_rcps[j], " "));    //print recipe child is executing
                        tokens_count = makeargv(mul_rcps[j], " ", &tokens); //child tokenizes recipes for exec
                        if(tokens_count == -1){
                            printf("Failed to tokenize in execute_stack\n");
                            return -1;
                        }
                        if(execvp(tokens[0],&tokens[0]) == -1){ //execute instruction and kill child if error
                            printf("Failed to change image of child to execute instruction.\n");
                            exit(1);        //kill child to avoid zombies
                        }
                    }
                }
                //parent code
                while(r_wait(NULL) > 0);    //have parent wait for all its children
            }
            been_visited(t);                //mark our target as visited so each target gets executed only once
        }
    }
    return 0;
}

/*
Prints a descriptive message on how to execute the program.
*/
void print_ussage_message(){
    printf("Incorrect usage of program.\n");
    printf("----------------------------------\n");
    printf("Correct ways to execute program:\n");
    printf("\t./mymake filename\n");
    printf("\t./mymake filename target \n");
    printf("\t./mymake -p filename\n");
    printf("\t./mymake -r filename\n");
}
