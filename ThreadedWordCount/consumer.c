/*
* test machine: CSELAB_machine_name 
* date: 11/07/2019
* name: Azul Bolaños, Adam Comerford
* x500: bolan108, comer059
 */

#include "header.h"

/*
    Picks the next element of a non-empty queue, evaluates the string of 
    the element, and updates the historgram accordingly. If queue is empty,
    it either waits for producer or exits if producer is done.
*/
void * consumeNode(void * id) {
    if(log_flag){
        if (pthread_mutex_lock(&buf_lock)){  //lock of queue also used for lock of log_fp
            printf("Consumer error when locking buf. Terminating program.\n");
            exit(1); //terminate program
        }
        /*******************************Log_fp critical section start********************************/
        fprintf(log_fp, "consumer %d\n", *(int *) id);  //signaling consumer is launched
        /*******************************Log_fp critical section end********************************/
        if(pthread_mutex_unlock(&buf_lock)){
            printf("Consumer error when unlocking buf. Terminating program.\n");
            exit(1); //terminate program
        }
    }
    while(1){
        if(pthread_mutex_lock(&buf_lock)){  //Get access to queue
            printf("Consumer error when locking buf. Terminating program.\n");
            exit(1); //terminate program
        }
        /*******************************Queue critical section start********************************/
        while(buf_count == 0){     //empty queue
            if(EOF_flag){       //Consumer is finished, since producer is done
                if(pthread_mutex_unlock(&buf_lock)){
                   printf("Consumer error when unlocking buf. Terminating program.\n");
                   exit(1); //terminate program
                }
                pthread_exit(NULL);
            }
            else        //wait for producer to fill
                if(pthread_cond_wait(&has_node, &buf_lock)){
                    printf("Consumer failed to wait. Terminating program\n");
                    exit(1);
                }
        }
        node * c_node = head; //get element of queue
        head = nextHead(head); //update queue
        buf_count--; //update count of elements on queue
        
        if(bounded_flag)
            if(buf_count == (buf_size - 1))
                if (pthread_cond_signal(&open_buf)){ //wake up producer waiting for space in buffer
                    printf("Consumer failed to signal. Terminating program\n");
                    exit(1);
                }
        
        if(log_flag)
            fprintf(log_fp, "consumer %d: %d\n", *(int *) id, c_node->num); 
        /*******************************Queue critical section end********************************/
        if(pthread_mutex_unlock(&buf_lock)){ //done using queue
            printf("Consumer error when unlocking buf. Terminating program.\n");
            exit(1); //terminate program
        }
        
        //processes line of text
        int alpha = 0;
        for(int i = 0; i < strlen(c_node->str); i++) {
            char current = (c_node->str)[i];
            if (isalpha(current) && !alpha) {
                current = tolower(current);
                if(pthread_mutex_lock(&hist_locks[current - 'a'])){ //get corresponding histogram mutex lock
                    printf("Consumer error when locking hist. Terminating program.\n");
                    exit(1); //terminate program
                }
                /*******************************Histogram critical section start********************************/
                hist[current - 'a']->number += 1;
                /*******************************Histogram critical section end********************************/
                if(pthread_mutex_unlock(&hist_locks[current - 'a'])){
                    printf("Consumer error when unlocking hist. Terminating program.\n");
                    exit(1); //terminate program
                }
                alpha = 1;
            }
            else if (!isalpha(current))
                alpha = 0;
        }
        freeNode(c_node);//element won't be used again
    }
}
