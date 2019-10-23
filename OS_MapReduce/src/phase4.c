#include "phase4.h"

/*
    Uses stdout redirection to print the read connter of the file
    in reducer_path to the file output_path. 
*/

int produceFinalOutput(char *reducer_path, char *output_path)
{
    char line[LINE_SIZE];
    FILE *output_fp;
    FILE *reducer_fp;
    //open output file to make a sytem call entry
    if ((output_fp = fopen(output_path, "a")) == NULL)
    {
        printf("Failed to open output file in produceFinalOutput\n");
        return -1;
    }
    //open reducer fie to read from
    if ((reducer_fp = fopen(reducer_path, "r")) == NULL)
    {
        printf("Failed to open output file in produceFinalOutput\n");
        return -1;
    }
    //redirect stdout to output file
    if (dup2(fileno(output_fp), STDOUT_FILENO) == -1)
    {
        printf("Failed to redirect standard output in produceFinalOutput\n");
        return -1;
    }
    fclose(output_fp); //output_fp no longer is needed

    while (fgets(line, LINE_SIZE, reducer_fp))
    {
        printf("%s", line);
    }
    fclose(reducer_fp);
    return 0;
}