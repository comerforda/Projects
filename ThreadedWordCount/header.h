/*
* test machine: CSELAB_kh4250-15
* date: 11/07/2019
* name: Azul Bolaños, Adam Comerford
* x500: bolan108, comer059
*/

#ifndef _HEADER_H_
#define _HEADER_H_

//for histogram
#ifndef ALPH_SIZE
#define ALPH_SIZE 26
#endif

//for buffer elements
#ifndef MAX_LINE_LENGTH
#define MAX_LINE_LENGTH 1024
#endif

#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct{
	char letter;
	int number;
} tuple;

typedef struct node node;
struct node{
    char * str;
    node * next;
    node * last;
    int num; //keeps track of line number, for log
};

extern node * head;  //the queue, producer write to it, consumers take from it
extern pthread_mutex_t buf_lock; //grants exclusive access to head, buf_count, EOF_flag,  and log_fp
extern pthread_mutex_t hist_locks[ALPH_SIZE]; //allows us to update mulitple distinct entries of histogram
extern int buf_count; //keeps track of number of elements in the queue
extern int buf_size; //(EC) bounded queue size
extern int EOF_flag; //signals producer is done
extern pthread_cond_t open_buf; //(EC) handles overflow
extern pthread_cond_t has_node; //handles underflow
extern int bounded_flag; //(EC) indicates working with a bounded buffer
extern tuple* hist[ALPH_SIZE];  // the histogram, where cosumers store results
extern FILE * log_fp;   //log file
extern int log_flag;    //indicate log has been requested

//main functions
int initThreads(pthread_t * p_tid,  pthread_t **tids, char * file_path, int c_count, int * c_ids);
int initProducer(pthread_t * p_tid, char * file_path);
int initConsumers(pthread_t **tids, int c_count, int * c_ids);
int joinThreads(pthread_t p_tid, pthread_t * tids, int c_count);
int prepare_log(int ** c_ids, int c_count);

//Histogram functions
int initTuples(tuple * letters[ALPH_SIZE]);

//Queue functions
struct node* createNode(char * line, int n);
void addNode(node * head, node * newNode);
struct node* nextHead(node * head);
void freeNode(node * n);

//Producer functions
void* produceNode(void * file_path);

//Consumer functions
void* consumeNode(void * id);



#endif
