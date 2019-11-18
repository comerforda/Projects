/*
* test machine: CSELAB_kh4250-15
* date: 11/07/2019
* name: Azul Bolaños, Adam Comerford
* x500: bolan108, comer059
*/

#include "header.h"

node * head = NULL;  //the queue, producer write to it, consumers take from it
pthread_mutex_t buf_lock = PTHREAD_MUTEX_INITIALIZER; //grants exclusive access to head, buf_count, EOF_flag, and log_fp
pthread_mutex_t hist_locks[ALPH_SIZE]; //allows us to update mulitple distinct entries of hist
int buf_count = 0; //keeps track of number of elements in the queue
int buf_size; //(EC) bounded queue size
int EOF_flag = 0; //signals producer is done
pthread_cond_t open_buf; //(EC) handles overflow
pthread_cond_t has_node; //handles underflow
int bounded_flag = 0; //(EC) indicates working with a bounded buffer
FILE * log_fp;   //log file
int log_flag = 0; //indicate log has been requested
tuple* hist[ALPH_SIZE]; // the histogram, where cosumers store results


int main(int argc, char * argv[]) {
    pthread_t *tids;  //consumer ids
    pthread_t p_tid; //producer id
    int c_count = 0; //num of consumers
    int * c_ids = NULL; //for log
    FILE * result_fp; //file pointer for result.txt

    /*  $ ./wcs #consumer filename [option] [#queue_size]   */
    //incorrect number of arguments:
    if (argc < 3) {
        printf("Argument mismatch, received: %d arguments, expected 3-5 \n", argc);
        printf("Correct program usage: ./wcs #ofconsumers filename [option] [#queue_size]\n");
        return 1;
    }
    else if (argc > 5) {
        printf("Argument mismatch, received: %d arguments, expected 3-5 \n", argc);
        printf("Correct program usage: ./wcs #ofconsumers filename [option] [#queue_size]\n");
        return 1;
    }
    //Correct number of arguments: prep for producer-consumer eval
    c_count = atoi(argv[1]);
    if (initTuples(hist)){ //create hist
        printf("Failed to initialize histogram\n");
        return 1;
    }
    //create hist locks
    for(int i = 0; i < ALPH_SIZE; i++){
        if(pthread_mutex_init(&hist_locks[i], NULL)){
            printf("Failed to initialized histogram locks\n");
            return 1;
        }
    }

    /*  $ ./wcs #consumer filename   */
    if (argc == 3) {
        if(initThreads(&p_tid, &tids, argv[2], c_count, c_ids)){
           perror("Failed to initalize threads\n");
           return 1;
        }
        if(joinThreads(p_tid,tids, c_count)){
            perror("Failed to join threads\n");
            return 1;
        }
    }
    else if (argc == 4) { /*  $ ./wcs #consumer filename -p   */
        if (strcmp(argv[3], "-p")) {
            printf("Incorrect option inputted: enter '-p' to execute log.\n");
            return 1;
        }
        if(prepare_log(&c_ids,c_count)){
            perror("Failed to setupt log\n");
            return 1;
        }
        if(initThreads(&p_tid, &tids, argv[2], c_count, c_ids)){
           perror("Failed to initalize threads\n");
           return 1;
        }
        if(joinThreads(p_tid,tids, c_count)){
            perror("Failed to join threads\n");
            return 1;
        }
    }
    //Extra credit: bounded buffer entered
    else if (argc == 5) {
        buf_size = atoi(argv[4]);
        bounded_flag = 1;
        /*  $ ./wcs #consumer filename -b #queue_size   */
        if (!strcmp(argv[3], "-b")) {  
            if(initThreads(&p_tid, &tids, argv[2], c_count, c_ids)){
                perror("Failed to initalize threads\n");
                return 1;
            }
            if(joinThreads(p_tid,tids, c_count)){
                perror("Failed to join threads\n");
                return 1;
            }
        }
        /*  $ ./wcs #consumer filename -bp #queue_size  */
        else if (!strcmp(argv[3], "-bp")) { 
            if(prepare_log(&c_ids,c_count)){
                perror("Failed to setupt log\n");
                return 1;
            }
            if(initThreads(&p_tid, &tids, argv[2], c_count, c_ids)){
                perror("Failed to initalize threads\n");
                return 1;
            }
            if(joinThreads(p_tid,tids, c_count)){
                perror("Failed to join threads\n");
                return 1;
            }
        }
        else {
            perror("Incorrect option given, correct options: '-b' or '-bp'\n");
            return 1;
        }
    }

    //put histogram into result.txt
    if( (result_fp = fopen("result.txt", "w")) == NULL ){
        printf("Failed to open result.txt\n");
        return 1;
    }

    for (int i = 0; i < ALPH_SIZE; i++)
        fprintf(result_fp, "%c: %d\n", tolower(hist[i]->letter), hist[i]->number);

}

//Sets up log flag and log file. Called if the -p or -bp options are used.
int prepare_log(int ** c_ids, int c_count){
    log_flag = 1;
    if( (*c_ids = (int *) malloc(sizeof(int) * c_count)) == NULL){
        perror("Failed to allocate c_ids for log\n"); 
        return 1;
    }
    for(int i = 0; i < c_count; i++)
        (*c_ids)[i] = i;
    if((log_fp = fopen("log.txt", "w")) == NULL){
        printf("Failed to create log file.\n");
        return 1;
    }
    return 0;
}

// Initializes a producer thread and c_count consumer threads. Returns non_zero value is unsuccesful.
int initThreads(pthread_t * p_tid,  pthread_t **tids, char * file_path, int c_count, int * c_ids){
    return initProducer(p_tid,file_path) + initConsumers(tids, c_count, c_ids);
}


//Creates producer thread, storing its id in p_tid. Returns 0 if successful, else 1.
int initProducer(pthread_t * p_tid, char * file_path){
    int error;
    //create producer thread
    if (error = pthread_create(p_tid, NULL, produceNode, (void *) file_path)) {
            fprintf(stderr, "Failed to start producer thread: %s \n", strerror(error));
            return 1;
    }
    return 0;
}

//Creates c_count consumer threads, storing its ids in tids. Returns 0 if successful, else 1.
int initConsumers(pthread_t **tids, int c_count, int * c_ids){
    int error;
    int* c_arg;
    //allocate space for consumer thread IDs
    if ((*tids = (pthread_t *) calloc (c_count, sizeof(pthread_t))) == NULL) {
        perror("Failed to allocate space for consumer thread IDs\n");
        return 1;
    }
    //create consumer threads
    for (int i = 0; i < c_count; i++) {
        if(log_flag)
            c_arg = &c_ids[i];
        else
            c_arg = NULL;

        if (error = pthread_create( *tids + i, NULL, consumeNode, (void *) c_arg)){
            fprintf(stderr, "Failed to start consumer %d: %s \n", i, strerror(error));
            return 1;
        }
    }

    return 0;
}

//joins the producer thread with id p_tid and c_count consumer threads whose ids are stored in tids
int joinThreads(pthread_t p_tid, pthread_t * tids, int c_count){
    int error;
    //join producer
    if (error = pthread_join(p_tid, NULL)){
        fprintf (stderr, "Failed to join producer thread: %s \n", strerror(error));
        return 1;
    }
    //join consumers
    for (int j = 0; j < c_count; j++) {
        if (error = pthread_join(tids[j], NULL)){
            fprintf (stderr, "Failed to join consumer thread: %s \n", strerror(error));
            return 1;
        }
    }
    return 0;
}
