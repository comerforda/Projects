#include "tuple.h"

/*
  Given an array of tuples, it mallocs memory for each tuple and initilazes them
  with a number of 0 and with a corresponding alphabet letter relative to their
  index in the array. If unsuccesful, it return -1, else it returns 0;
*/
int initTuples(tuple * letters[ALPH_SIZE])
{	//remember to free tuples!!
	for (int i = 0; i < ALPH_SIZE; i++)
	{
		if ( (letters[i] = (tuple *) malloc(sizeof(tuple))) == NULL )
        {
            printf("Failed to allocated memory in initTuples\n");
            return -1;
        }
		letters[i]->number = 0;
		letters[i]->letter = 'A' + i;
	}
    return 0;
}
/*
  Given a valid letter, it increases the corresponding letters entry by num.
  If invalid letter or num values are given, -1 is returned, else 0 is returned.
*/
int addLetter(tuple * letters[], char letter, int num)
{
    if(num < 1) //function increases count only
        return -1;
	if((letter >= 'a' && letter <= 'z') || (letter >= 'A' && letter <= 'Z'))
	{
		for (int i = 0; i < ALPH_SIZE; i++)
		{
			if (letters[i]->letter == letter || letters[i]->letter == (letter - 32))
			{
                (letters[i]->number) += num;
                return 0;   
            }
		}
	}
    return -1; //invalid letter character

}

/*
  Frees all malloced entries in letters array
*/
void freeTuples(tuple * letters[ALPH_SIZE])
{
    for (int i = 0; i < ALPH_SIZE; i++)
	{
		free(letters[i]);
	}
}