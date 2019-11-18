/*
* test machine: CSELAB_kh4250-15
* date: 11/07/2019
* name: Azul Bolaños, Adam Comerford
* x500: bolan108, comer059
*/

#include "header.h"

/*
    Produces elements from the file specified in file_path and puts
    them on the queue. If working with a bounded buffer, producer waits
    for a consumer if the queue is full. The producer finishes when reaching
    the EOF of the file_path file and sets up the EOF_flag that lets consumers 
    know nothing else will be produced.
*/
void* produceNode(void * file_path){
    char file_line[MAX_LINE_LENGTH];
    node * new_node; 
    FILE * fp;
    int line_counter = 0;

	if((fp = fopen( ((char*) file_path), "r")) == NULL){
		printf("Failed to open file: %s \n", (char*) file_path);
        exit(1); //terminate program
	}
    if(pthread_mutex_lock(&buf_lock)){
        printf("Producer error when locking buf. Terminating program.\n");
        exit(1);
    }
        

    /*******************************Buffer critical section start********************************/
    while(fgets(file_line, MAX_LINE_LENGTH, fp)){
        if(!(line_counter) && log_flag)
            fprintf(log_fp,"producer\n");
        if(bounded_flag)
            if(buf_count == buf_size) 
                if(pthread_cond_wait(&open_buf,&buf_lock)){
                    printf("Producer failed to wait. Terminating program.\n");
                    exit(1);
                }
        
        if(log_flag)
            fprintf(log_fp, "producer: %d\n", line_counter);
   
        char * line;
        if((line = (char *) malloc(sizeof(char) * (strlen(file_line) + 1) )) == NULL){
            printf("Producer failed to allocate line %d\n", line_counter + 1);
            exit(1);
        }
        strcpy(line, file_line);
        if((new_node = createNode(line, line_counter++)) == NULL){
            printf("Producer failed to create node for line %d: %s\n", line_counter - 1, line);
            exit(1);
        }
        
        if(head == NULL)
            head = new_node;
        else
            addNode(head, new_node);
        buf_count++;
        if(buf_count == 1)
            if(pthread_cond_broadcast(&has_node)){ //wake up consumers that are waiting for input
                printf("Producer failed to broadcast. Terminating program.\n");
                exit(1);
            }
    }
    EOF_flag = 1; //indicates producer is done

    /*******************************Buffer critical section end********************************/
    if(pthread_mutex_unlock(&buf_lock)){
        printf("Producer error when unlocking buf. Terminating program.\n");
            exit(1); //terminate program
    }
    return 0;
}
