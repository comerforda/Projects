Test machine: CSELAB kh4250 15
date: 09/30/2019
name: Azul Bolanos, Adam Comerford
x500: bolan108, comer059

The purpose of this program is to create our own executable version of the 'make' command, called 'mymake'.
mymake reads in a makefile and builds any target files specefied by executing their corresponding instructions.
 
When 'mymake' is executed, it takes up to two arguments with the following structure:
	./mymake filename
	./mymake filename [target]
	./mymake [-p] filename
	./mymake [-r] filename

If mymake is executed without a provided target, it starts with the first target and builds any corresponding dependencies in a BFS manner.
After buidling the targets and dependencies, the program will then execute and print their instructions in order.
If mymake is executed with a provided target, it will start with the specified target and build any corresponding dependcies in a BFS manner.
After buidling the targets and dependencies, the program will then execute and print their instructions in order.
If mymake is executed with the '-p' flag, it will print a list of the targets, dependencies, and instructions in the makefile.
If mymake is executed with the '-r' flag, it will print out the order in which instructions will be executed.
If either flag is used, the instructions will not be executed.

To compile 'mymake', a makefile, 'MasterMake' has been attached with the program source files
Run: $ make -f MasterMake
the mymake executable should be created. Then you may run any of the previously stated commands, provided that mymake is
within the same directory as the inputted filename.

Extra credit: The program effectively handles parallel execution of recipes and multiples recipes per target, as can be seen
	      in the 'execute_stack' function. Kills child processes when done executing and parent waits for them to correctly 
	      avoid a zombie process.

Assumptions:
1.) We should print the instructions as they are being executed to mimick how the make command behaves.

2.)For the extra credit, we assumed that -p will print instructions with commas as single instructions. For example, if given the following target:
Target1: 
	mkdir myproject, pwd
Then using our program with -p file will output the following:
Target 'Target1' has 0 dependencies and 1 recipe(s)
Recipe 0 is mkdir myproject, pwd

This assumption was necessary so that after building all our target arrays, we could still identify which target instructions could run in parallel. 
Flag -r would then print the recipe order as:
mkdir myproject
pwd
The order might vary based on the instructions and which child finishes first.

3.)If an instruction fails we are not expected to quit executing the rest. 

Contributions:
Azul: Majority of program engineering including:
	Parsing input makefile into target array
	Handling of data structures
	Error handling
	Extra credit conditions
	Output

Adam: Program structure and design:
	Documentation and stylization (cleaning up code, commenting, README)
	Implementation of stack and exec()
	Input conditions
	Modularization

Neither list of contributions is exclusive to that member. 
Both members heavily involved in discussion, planning, and research.
