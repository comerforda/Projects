/*
Test machine: KH 4-250
date: 10/19/2019
name: Azul Bolaños, Adam Comerford
x500: bolan108, comer059
*/
#include "main.h"

int main(int argc, char *argv[])
{

    // argument count check
    if (argc != 3)
    {
        printf("Wrong number of arguments. Expected 2, received %d\n", argc - 1);
        exit(1);
    }

    int m = atoi(argv[2]);
    int pipes[m * 2];
    char input_dir[MAX_PATH];
    char reducer_path[MAX_PATH];
    char output_path[MAX_PATH];
    int file_count;
    input_dir[0] = '\0';
    reducer_path[0] = '\0';
    output_path[0] = '\0';

    //setup paths
    getcwd(input_dir, MAX_PATH);
    strcat(input_dir, "/MapperInput");

    getcwd(reducer_path, MAX_PATH);
    strcat(reducer_path, "/");
    strcat(reducer_path, "ReducerResult.txt");

    getcwd(output_path, MAX_PATH);
    strcat(output_path, "/");
    strcat(output_path, "FinalResult.txt");

    //phase1 - Data Partition Phase
    file_count = makeMapperInput(argv[1], input_dir, m);
    if(file_count == 0)
        printf("No file read in %s\n", argv[1]);
    else if(file_count == -1)
    {
        printf("Failed to complete Phase 1\n");
        return -1;
    }
    else
    {
        //create pipes
        for (int i = 0; i < m; i++)
        {
            if( pipe(pipes + (2 * i)) == -1)
            {
                printf("Failed to create pipe\n");
                return -1;
            }
        }
            

        //phase2 - Map Function
        setUpMappers(pipes, m, input_dir);

        //phase3 - Reduce Function
        reduce(pipes, m, reducer_path);

        //phase4
        //master process reports the final output
        if(produceFinalOutput(reducer_path, output_path))
        {
            printf("Failed to complete Phase 4\n");
            return -1;
        }
    }
    return 0;
}