/* Dependency of main.c - functions to parse and tokenize input.
   Provided by either book or in assignment zip*/

#include "helper.h"

/*
Parses the file into individual strings and stores them into lines array.
Returns the number of strings the parsing generates.
*/
int process_file(char *fname){
	FILE* fp = fopen(fname, "r");
	char line[LINE_SIZE];
	int i = 0;
    int count = 0;

	if (!fp) {
		printf("Failed to open the file: %s \n", fname);
		return -1;
	}

	//Read the contents and store in lines
	while (fgets(line, LINE_SIZE, fp)){
		strncpy(lines[i++], line, strlen(line));
        count++;
    }

	fclose(fp);

	return count;
}

/* 
Tokenizes an input string based on the specified delimiters.
Stores tokens in new array and returns number of tokens.
*/
int makeargv(const char *s, const char *delimiters, char ***argvp) {
   int error;
   int i;
   int numtokens;
   const char *snew;
   char *t;

   if ((s == NULL) || (delimiters == NULL) || (argvp == NULL)) {
      errno = EINVAL;
      return -1;
   }
   *argvp = NULL;                           
   snew = s + strspn(s, delimiters);         /* snew is real start of string */
   if ((t = malloc(strlen(snew) + 1)) == NULL) 
      return -1; 
   strcpy(t, snew);               
   numtokens = 0;
   if (strtok(t, delimiters) != NULL)     /* count the number of tokens in s */
      for (numtokens = 1; strtok(NULL, delimiters) != NULL; numtokens++) ; 

                             /* create argument array for ptrs to the tokens */
   if ((*argvp = malloc((numtokens + 1)*sizeof(char *))) == NULL) {
      error = errno;
      free(t);
      errno = error;
      return -1; 
   } 
                        /* insert pointers to tokens into the argument array */
   if (numtokens == 0) 
      free(t);
   else 
   {
      strcpy(t, snew);
      **argvp = strtok(t, delimiters);
      for (i = 1; i < numtokens; i++)
          *((*argvp) + i) = strtok(NULL, delimiters);
    } 
    *((*argvp) + numtokens) = NULL;             /* put in final NULL pointer */
    return numtokens;
}

/* function for parent to wait for children even if blocking signal is received */
pid_t r_wait(int *stat_loc){
	int retval;
	while( ((retval = wait(stat_loc)) == -1) &&  (errno == EINTR) );
	return retval;
}