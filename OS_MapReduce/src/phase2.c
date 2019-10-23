#include "phase2.h"

/*
  Creates mapper processes each which creates an array of the number of words, 
  per letter of the alphabet, in the files listed in their respective m mapperfile 
  located in input_dir. After going through all its files, each mapper process
  writes the final letter counts into a pipe.
*/
int setUpMappers(int *pipes, int m, char *input_dir)
{
	int childpid;

	for (int i = 0; i < m; i++)
	{
		if ((childpid = fork()) == -1)
		{
			printf("Failed to create mapper process\n");
			return -1;
		}
		if (!childpid) //mapper(child) code
		{
			tuple *letters[ALPH_SIZE];
			char mapper_path[MAX_PATH];
			int mapper_pipe = 2 * i + 1;
			if(initTuples(letters) == -1)
            {
                printf("Failed to initialize tuples in setUpMappers\n");
                exit(1);
            }
			//setup mapper with corresponding writing pipe
			setMapperPipe(pipes, m, mapper_pipe);
			//process files of mapper
			getMapperPath(mapper_path, input_dir, i + 1);
			if(processText(mapper_path, letters) == -1)
			{
				printf("Failed to process files in %s\n", mapper_path);
				freeTuples(letters);
				exit(1);
			}

			//write data to pipes
			for (int j = 0; j < ALPH_SIZE; j++)
			{
				if( (write(pipes[mapper_pipe], letters[j], sizeof(tuple))) == -1)
				{
					printf("Mapper failed to write to pipe %d\n", mapper_pipe);
					return -1;
				}
			}
			freeTuples(letters);
			close(pipes[mapper_pipe]);
			exit(0); //nothing left for mapper to do
		}
	}
	//master code
	while (r_wait(NULL) > 0); //wait for Mappers to finish writting
	closeWritingPipes(pipes, m); //will cause reducer to have proper pipes
}

int processText(char *mapper_path, tuple *letters[])
{
	char line[LINE_SIZE];
	char txt_path[MAX_PATH];

	//open mapper_i file for reading, returns -1 if failed
	FILE *mapper_fp;
	if( (mapper_fp = fopen(mapper_path, "r")) == NULL)
	{
		printf("Failed to open mapper path: %s \n", mapper_path);
		return -1;
	}

	//iterate over all the txt paths in mapper_i
	while (fgets(txt_path, MAX_PATH, mapper_fp))
	{
		//remove the newline and 'read' character from the txt_path string.
		txt_path[strcspn(txt_path, "\r\n")] = 0;

		//open each txt file path, return -1 if it fails
		FILE *fp;
		if( (fp = fopen(txt_path, "r")) == NULL )
		{
			printf("Failed to open file path:%s\n", txt_path);
			return -1;	
		}

		//adds 1st letter of each word to each mapper's letters array
		while (fgets(line, LINE_SIZE, fp))
		{
			addLetter(letters, line[0], 1);
		}
		fclose(fp);
	}
	fclose(mapper_fp);
	return 0;
}

/*
	Closes all ends of the m pipes in the pipes array save for the 
	spcified mapper_pipe end.
*/
int setMapperPipe(int *pipes, int m, int mapper_pipe)
{
	for (int i = 0; i < 2 * m; i++)
	{
		if (i != mapper_pipe) //close all pipes except corresponding writing pipe
			close(pipes[i]);
	}
}

/*
	Closes all corresponding writing ends of the m pipes of in the array pipes
*/
int closeWritingPipes(int *pipes, int m)
{
	for (int i = 1; i < 2 * m; i++)
	{
		if (i % 2) //all the writing pipes are odd
		{
			close(pipes[i]);
		}
	}
}
