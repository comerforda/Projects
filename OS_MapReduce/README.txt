Test machine: KH 4-250
date: 10/19/2019
name: Adam Comerford, Azul Bolaños
x500: comer059, bolan108

Program description:
This program is a basic implementation of the MapReduce model of programming.
Given a multi-level folder hierarchy containing folders and text files, and an an integer, m ,
this program traverses the folder hierarchy and stores the text file paths into 'm' mapper_i.txt
files (in a round-robin fashion) within a new directory 'MapperInput'. Then it opens each of those paths 
and counts the individual letters that each word (one per line) begins with.
It does this by spawning m 'mapper' processes which open and evaluate each text file. Then,
the mappers send the data to a 'reducer' process, which combines the results of each mapper
and outputs them to a file, 'FinalResult.txt' in the current working directory.

Compilation:
Go to the src directory and simply run:
    $ make

This will create an executable, 'mapreduce', then to run the program:
    $ ./mapreduce filepath m

Where 'filepath' is the path of the folder hierarchy you wish to test, and 'm' is 
the number of mapper processes you wish to spawn.

Assumptions:
    1.) This program does not remove the 'MapperInput', 'ReducerResult.txt', or 'FinalResult.txt'
        files after it is finished. In order to pass the test cases, these files must be removed
        before a new test is run (this was confirmed acceptable by TA).
        Consequences of files not being removed:
        - MapperInput: Claims that phase 1 was failed becase MapperInput wasn't created.
        - ReducerResult/FinalResult: The previous test's output will be added to the current output file.
        
    2.) The mapper_i files begin with i = 1 to m.
    3.) We made new 'tuple.c' 'tuple.h' and 'helper.c' 'helper.h' files to store our data structure 
        for the <key,value> pairs and commonly used functions, then modified the makefile to accomadate 
        for the extra modules.

Requested mapper_i partition logic:
    Cycle through the mapper_i files in a round-robin fashion from the start:
    1.) Start at mapper = 1 and get the mapper_i path by calling getMapperPath (defined: "helper.c")
    2.) Open the mapper_i path and write to it, then increment 'mapper'
    3.) Repeat until mapper == m , then set mapper = 1 , effectively looping
        from mapper_1 to mapper_m.

Contributions:
    Each team member was heavily involved in planning, research, and engineering
    and neither list of contributions is exclusive to that member.

    Adam Comerford:
        - File hierarchy traversal
        - MapperInput/mapper_i files
        - Partitioning logic
        - Tuple (<key, value>) data structure/operations
        - Stylization, documentation (README)
        - Modularization

    Azul Bolaños:
        - Map/Reduce functions
        - Piping between Mappers and Reducer
        - File redirection
        - Error handling
        - Commenting & Modularization

We did not attempt the extra credit.