#include "phase3.h"

/*
	Reducer function: reducer process reads from all m pipes in the 
	pipes array and writes results into the file in reducer_path.
*/
int reduce(int *pipes, int m, char *reducer_path)
{
	int childpid;
	if((childpid = fork()) == -1)
	{
		printf("Failed to create reducer process\n");
		return -1;
	}

	if (!childpid) //reducer code
	{
		tuple *letters[ALPH_SIZE];
		tuple *t;
		if( (t = (tuple *)malloc(sizeof(tuple))) == NULL )
		{
			printf("Failed to malloc tuple pointer in reducer\n");
			exit(1);
		}

		if (initTuples(letters) == -1)
        {
            printf("Failed to initialized tuples in reduce\n");
            exit(1);
        }
		//read pipes data
		for (int i = 0; i < m; i++)
		{
			for (int j = 0; j < ALPH_SIZE; j++)
			{
				read(pipes[2 * i], t, sizeof(tuple));
				addLetter(letters, t->letter, t->number);
			}
		}
		//write final letter count
		FILE *fp;
		if( (fp = fopen(reducer_path, "a")) == NULL )
		{
			printf("Failed to open reducer path\n");
			freeTuples(letters);
			exit(1);
		}
		for (int i = 0; i < ALPH_SIZE; i++)
		{
			fprintf(fp, "%c %d\n", letters[i]->letter, letters[i]->number);
		}
		
		//free reducer resources
		free(t);
		fclose(fp);
		freeTuples(letters);
		closeReadingPipes(pipes, m);
		exit(0);
	}
	closeReadingPipes(pipes, m);
	r_wait(NULL); //wait for reducer to finish
	return 0;
}

/*
	Closes all corresponding reading ends of the m pipes of in the array pipes
*/
int closeReadingPipes(int *pipes, int m)
{
	for (int i = 0; i < 2 * m; i++)
	{
		if (!(i % 2))
		{ //all the writing pipes are even or 0
			close(pipes[i]);
			//printf("closed reading pipe %d\n", i);
		}
	}
}
