test machine: CSELAB_kh4250-15
date: 11/07/2019
name: Azul Bolaños, Adam Comerford
x500: bolan108, comer059

Description:

This project, "Word Count Statistic" is an implementation of the Producer-Consumer model
executed in a synchronized, multi-threaded fashion.
Given a text file, it reads and counts the first letter of every word in the file, storing the 
count of each letter into a shared histogram, which is then outputted to a file, "result.txt" in
the current working directory.
It does this by creating n 'Consumer' threads (specified by user) which read lines of text from
a shared queue (linked list buffer - bounded/unbounded - specified by user) which the Producer provides
by traversing and reading the file. While reading through their lines, the Consumers increment the count
of the first letter of each word in a shared histogram.
The problem focused on figuring out how to synchronize the Producer adding lines to the queue
and the Consumers reading from the queue. We solved this by using mutex locks and condition variables
which allowed mutual exclusion to the shared queue. The Producer effectively waits for Consumers when
the buffer is full (if bounded buffer specified), and the Consumers effectively wait for the Producer
to add to the queue when empty.
Instead of enforcing mutual exclusion for the shared histogram, we opted for mutual exclusion on each 
element of the histogram. That is, every Consumer can access the histogram, but only one can modify a 
specific letter at a time. This makes it so all of the Consumer threads work in parallel, significantly
increasing the rate at which they can work.

Extra Credit: 
The program effectively implements and operates on a bounded buffer.

Compilation:
The source files include a makefile, simply run:
    $ make

This will create an executable, 'wcs' (for "word count statistic").
wcs takes the following arguments and syntax:
    $ ./wcs #consumer filename [option] [queue_size]

- '#consumer' is the number of consumers the program will create.
- 'filename' is the txt file you wish to analyze.
- [option] has three possibilities: "-p", "-b", "-bp"
    1.) "-p" creates a log which prints the thread execution information to a txt file, "log.txt"
    2.) "-b" specifies a bounded buffer for the shared queue (Extra Credit)
    3.) "-bp" specifies a bounded buffer and creates a log
- [queue_size] specifies the size of the bounded buffer, should only be used with "-b" or "-bp" options.

NOTE: Contrarly to the provided make file, our make also removes 'result.txt' and 'log.txt' when run with 
$ make clean

Assumptions:
1.) The filename is either in the current working directory, or a path is inputted.
2.) One line of input file has at most 1024 characters.

Contributions:
Both group members heavily involved in planning/engineering, and neither list of contributions
is exclusive to that member.

Azul Bolaños:
    - Producer functionality
    - Linked List buffer implementation
    - Producer/Consumer synchronization (mutex/condition variables)
    - Commenting
    - Modularization
    - File output

Adam Comerford:
    - Thread logic (creation/joining, error handling)
    - Histogram implementation
    - Consumer logic/functionality
    - Input conditions
    - deliverables (README)
