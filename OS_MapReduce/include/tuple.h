#ifndef TUPLE_H
#define TUPLE_H

#include <stdio.h>
#include <stdlib.h>

#include "helper.h"

typedef struct{
	char letter;
	int number;
} tuple;


int initTuples(tuple * letters[ALPH_SIZE]);
int addLetter(tuple * letters[], char letter, int num);
void freeTuples(tuple * letters[ALPH_SIZE]);

#endif //TUPLE_H