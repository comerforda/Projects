/*
* test machine: CSELAB_kh4250-15
* date: 11/07/2019
* name: Azul Bolaños, Adam Comerford
* x500: bolan108, comer059
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"
// pthread.h included in header.h

// implement shared queue, final histogram here.. 


/**********************************HISTOGRAM*******************************************/
/*
  Given an array of tuples, it mallocs memory for each tuple and initilazes them
  with a number of 0 and with a corresponding alphabet letter relative to their
  index in the array. If unsuccesful, it return -1, else it returns 0;
*/
int initTuples(tuple * letters[ALPH_SIZE])
{	//remember to free tuples!!
	for (int i = 0; i < ALPH_SIZE; i++){
		if ( (letters[i] = (tuple *) malloc(sizeof(tuple))) == NULL ){
            printf("Failed to allocated memory in initTuples\n");
            return 1;
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

/********************************** QUEUE *******************************************/

/*
  Creates a new node for the queue with str line and num n
*/
struct node* createNode(char * line, int n){
  struct node* temp;
  if( (temp = (struct node *) malloc(sizeof(struct node))) == NULL)
    return NULL;
  (temp->str) = line;
  temp->next = NULL;
  temp->last = temp;
  temp->num = n;
  return temp;
}

/*
  Adds element to the end of the queue and updates the head's tail pointer accordingly
*/
void addNode(node * head, node * new_node){
    node * tail = head->last;
    node * next_temp = head->next;
    head->last = new_node;
    if(next_temp == NULL){ //only have one element on the list, the head
      head->next = new_node;
    }else{
      tail->next = new_node;
    }
}

/*
  Returns the next element of the queue, and updates the last pointer of this
  element so it can be used as the head.
*/
struct node* nextHead(node * head){
    node * tail = head->last;
    node * new_head = head->next;
    if(new_head != NULL){
      new_head->last = tail;
    }
    return new_head;
}

// frees queue element n
void freeNode(node * n){
  free(n->str);
  free(n);
}





