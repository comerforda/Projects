#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
//#include <zconf.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>
#include "../include/protocol.h"

FILE *logfp;

//sets up rqst buffer for the indicated rqst_type
int setRequest(int * rqst, int map_id, int rqst_type){
    memset(rqst, 0, REQUEST_MSG_SIZE * sizeof(int));
    rqst[0] = rqst_type;
    rqst[1] = map_id;
    return 0;
}

void createLogFile(void) {
    pid_t p = fork();
    if (p==0)
        execl("/bin/rm", "rm", "-rf", "log", NULL);

    wait(NULL);
    mkdir("log", 0777);
    logfp = fopen("log/log_client.txt", "w");
}

pid_t r_wait(int *stat_loc){
	int retval;
	while( ((retval = wait(stat_loc)) == -1) &&  (errno == EINTR) );
	return retval;
}

//handles the deterministic request handling of mapper processes
int mapperCode(int map_id, char *mapper_path, char *server_ip, int server_port, FILE * logfp)
{
    //Connecting to the server
    // Create a TCP socket.
    int sockfd = socket(AF_INET , SOCK_STREAM , 0);
    // Specify an address to connect to (we use the local host or 'loop-back' address).
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(server_port);
    address.sin_addr.s_addr = inet_addr(server_ip); //for local host 127.0.01


    // Connect it.
    if(connect(sockfd, (struct sockaddr *) &address, sizeof(address)) == 0) {
        int request[REQUEST_MSG_SIZE];
        int response[LONG_RESPONSE_MSG_SIZE];
        memset(response, 0, REQUEST_MSG_SIZE * sizeof(int));
        char logTxt[LINE_SIZE]; //mapper file txt
        char line[LINE_SIZE];
	    char txt_path[MAX_PATH];
        FILE *mapper_fp; //fp for mapper file
        FILE *fp;// fp for files within mapper file

        //writte connection to log
        logTxt[0] = '\0';
        sprintf(logTxt, "[%d] open connection\n", map_id);
        fputs(logTxt,logfp);
        
        /***************************Checkin start****************************/
        setRequest(request, map_id, CHECKIN);
        do{
            write(sockfd, request, sizeof(request));
            read(sockfd, response, sizeof(response));
        }while(response[1] != RSP_OK);
        //writte checkin to log
        logTxt[0] = '\0';
        sprintf(logTxt, "[%d] CHECKIN: %d %d\n", map_id, response[1], response[2]);
        fputs(logTxt,logfp);
        /***************************Checkin end****************************/

        /***************************Update AZ-List start****************************/
        //open mapper file
        if((mapper_fp = fopen(mapper_path, "r")) == NULL)
        {
            printf("Failed to open mapper file: %s \n", mapper_path);
            close(sockfd);
            exit(1);
        }
        //iterate over all the txt paths in mapper file
        int rqst_count = 0;
        while (fgets(txt_path, MAX_PATH, mapper_fp))
        {
            //remove the newline and 'read' character from the txt_path string.
            txt_path[strcspn(txt_path, "\r\n")] = 0;
            //set up request
            setRequest(request, map_id, UPDATE_AZLIST);
            //open each txt file path, return -1 if it fails
            if( (fp = fopen(txt_path, "r")) == NULL )
                printf("Failed to open file path:%s\n", txt_path);

            //adds 1st letter of each word to request array
            while (fgets(line, LINE_SIZE, fp))
            {
                char letter = tolower(line[0]);
                request[ letter - 'a' + 2]++;  
            }
            //send request for the file fp processesd
            do{
                send(sockfd, request, sizeof(request), 0);
                recv(sockfd, response, sizeof(response), 0);
            }while(response[1] != RSP_OK);
            rqst_count++;
            fclose(fp);
        }
        //write update_azlist to log
        logTxt[0] = '\0';
        sprintf(logTxt, "[%d] UPDATE_AZLIST: %d\n", map_id, rqst_count);
        fputs(logTxt,logfp);
        /***************************Update AZ-List end****************************/

        /***************************Get AZ-List start****************************/
        setRequest(request, map_id, GET_AZLIST);
        do{
            send(sockfd, request, sizeof(request), 0);
            recv(sockfd, response, sizeof(response), 0);
        }while(response[1] != RSP_OK);

        //write get_azlist to log
        logTxt[0] = '\0';
        sprintf(logTxt, "[%d] GET_AZLIST: %d", map_id, response[1]);
        fputs(logTxt, logfp);
        for(int j = 2; j < LONG_RESPONSE_MSG_SIZE; j++){
            char element[3];
            element[0] ='\0';
            sprintf( element, " %d", response[j]);
            fputs(element, logfp);
        }
        fputs("\n",logfp);
        /***************************Get AZ-List end****************************/

        /***************************Get Mapper Updates start*************************/
        setRequest(request, map_id, GET_MAPPER_UPDATES);
        do{
            send(sockfd, request, sizeof(request), 0);
            recv(sockfd, response, sizeof(response), 0);
        }while(response[1] != RSP_OK);

        //write get_mapper_update to log
        logTxt[0] = '\0';
        sprintf(logTxt, "[%d] GET_MAPPER_UPDATES: %d %d\n", map_id, response[1], response[2]);
        fputs(logTxt,logfp);
        /***************************Get Mapper Updates end****************************/

        /***************************Get All Updates start****************************/
        setRequest(request, map_id, GET_ALL_UPDATES);
        do{
            send(sockfd, request, sizeof(request), 0);
            recv(sockfd, response, sizeof(response), 0);
        }while(response[1] != RSP_OK);

        //write get_all_update to log
        logTxt[0] = '\0';
        sprintf(logTxt, "[%d] GET_ALL_UPDATES: %d %d\n", map_id, response[1], response[2]);
        fputs(logTxt,logfp);
        /***************************Get All Updates end****************************/

        /***************************Checkout start****************************/
        setRequest(request, map_id, CHECKOUT);
        do{
            send(sockfd, request, sizeof(request), 0);
            recv(sockfd, response, sizeof(response), 0);
        }while(response[1] != RSP_OK);

        //write checkout to log
        logTxt[0] = '\0';
        sprintf(logTxt, "[%d] CHECKOUT: %d %d\n", map_id, response[1], response[2]);
        fputs(logTxt,logfp);
        /***************************Checkout end****************************/

        //close connection
        close(sockfd);

        //write close to log
        logTxt[0] = '\0';
        sprintf(logTxt, "[%d] close connection\n", map_id);
        fputs(logTxt,logfp);

        //mapper has finished
        exit(0);
	} else {
        fprintf(stderr, "[%d] connection failed!\n", map_id);
        exit(1);
	}
}

//handles the dynamic request handling of master process
int masterCode(char *server_ip, int server_port, FILE * logfp){
    FILE * command_fp;
    char command[10]; 
    int request[REQUEST_MSG_SIZE];
    int response[LONG_RESPONSE_MSG_SIZE];
    int map_id = MASTER_ID;
    char logTxt[LINE_SIZE]; 
    int sockfd;
    struct sockaddr_in address;


    memset(response, 0, REQUEST_MSG_SIZE * sizeof(int));
    //open command.txt file
    if((command_fp = fopen("commands.txt", "r")) == NULL)
    {
        printf("Failed to open mapper file: %s \n", "command.txt");
        exit(1);
    }
    while (fgets(command, 10, command_fp))
    {
        //remove the newline and 'read' character from the txt_path string.
        command[strcspn(command, "\r\n")] = 0;
        int rqst = atoi(&command[0]);
        switch(rqst) {
            case CHECKIN:
                //Connecting to the server
                // Create a TCP socket.
                sockfd = socket(AF_INET , SOCK_STREAM , 0);
                // Specify an address to connect to (we use the local host or 'loop-back' address).
                address.sin_family = AF_INET;
                address.sin_port = htons(server_port);
                address.sin_addr.s_addr = inet_addr(server_ip); //for local host 127.0.01
                if(connect(sockfd, (struct sockaddr *) &address, sizeof(address)) == 0){
                    //writte connection to log
                    logTxt[0] = '\0';
                    sprintf(logTxt, "[%d] open connection\n", map_id);
                    fputs(logTxt,logfp);
                    
                    setRequest(request, map_id, CHECKIN);
                    write(sockfd, request, sizeof(request));
                    read(sockfd, response, sizeof(response));
                    //writte checkin to log
                    logTxt[0] = '\0';
                    sprintf(logTxt, "[%d] CHECKIN: %d %d\n", map_id, response[1], response[2]);
                    fputs(logTxt,logfp);
                }
                close(sockfd);
                //write close to log
                logTxt[0] = '\0';
                sprintf(logTxt, "[%d] close connection\n", map_id);
                fputs(logTxt,logfp);
                break;
            case UPDATE_AZLIST:
                break; //should do nothing (we are allowed to assume it won't appear on command list anyway)
            case GET_AZLIST:
                //Connecting to the server
                // Create a TCP socket.
                sockfd = socket(AF_INET , SOCK_STREAM , 0);
                // Specify an address to connect to (we use the local host or 'loop-back' address).
                address.sin_family = AF_INET;
                address.sin_port = htons(server_port);
                address.sin_addr.s_addr = inet_addr(server_ip); //for local host 127.0.01
                if(connect(sockfd, (struct sockaddr *) &address, sizeof(address)) == 0){
                    //writte connection to log
                    logTxt[0] = '\0';
                    sprintf(logTxt, "[%d] open connection\n", map_id);
                    fputs(logTxt,logfp);
                    
                    setRequest(request, map_id, GET_AZLIST);
                    do{
                        send(sockfd, request, sizeof(request), 0);
                        recv(sockfd, response, sizeof(response), 0);
                    }while(response[1] != RSP_OK);
                    //write get_azlist to log
                    logTxt[0] = '\0';
                    sprintf(logTxt, "[%d] GET_AZLIST: %d", map_id, response[1]);
                    fputs(logTxt, logfp);
                    for(int j = 2; j < LONG_RESPONSE_MSG_SIZE; j++){
                        char element[3];
                        element[0] ='\0';
                        sprintf( element, " %d", response[j]);
                        fputs(element, logfp);
                    }
                    fputs("\n",logfp);
                }
                close(sockfd);
                //write close to log
                logTxt[0] = '\0';
                sprintf(logTxt, "[%d] close connection\n", map_id);
                fputs(logTxt,logfp);
                break;
            case GET_MAPPER_UPDATES:
                //Connecting to the server
                // Create a TCP socket.
                sockfd = socket(AF_INET , SOCK_STREAM , 0);
                // Specify an address to connect to (we use the local host or 'loop-back' address).
                address.sin_family = AF_INET;
                address.sin_port = htons(server_port);
                address.sin_addr.s_addr = inet_addr(server_ip); //for local host 127.0.01
                if(connect(sockfd, (struct sockaddr *) &address, sizeof(address)) == 0){
                    //writte connection to log
                    logTxt[0] = '\0';
                    sprintf(logTxt, "[%d] open connection\n", map_id);
                    fputs(logTxt,logfp);
                    
                    setRequest(request, map_id, GET_MAPPER_UPDATES);
                    send(sockfd, request, sizeof(request), 0);
                    recv(sockfd, response, sizeof(response), 0);
                    //write get_mapper_update to log
                    logTxt[0] = '\0';
                    sprintf(logTxt, "[%d] GET_MAPPER_UPDATES: %d %d\n", map_id, response[1], response[2]);
                    fputs(logTxt,logfp);
                }
                close(sockfd);
                //write close to log
                logTxt[0] = '\0';
                sprintf(logTxt, "[%d] close connection\n", map_id);
                fputs(logTxt,logfp);
                break;
            case GET_ALL_UPDATES:
                //Connecting to the server
                // Create a TCP socket.
                sockfd = socket(AF_INET , SOCK_STREAM , 0);
                // Specify an address to connect to (we use the local host or 'loop-back' address).
                address.sin_family = AF_INET;
                address.sin_port = htons(server_port);
                address.sin_addr.s_addr = inet_addr(server_ip); //for local host 127.0.01
                if(connect(sockfd, (struct sockaddr *) &address, sizeof(address)) == 0){
                    //writte connection to log
                    logTxt[0] = '\0';
                    sprintf(logTxt, "[%d] open connection\n", map_id);
                    fputs(logTxt,logfp);
                    setRequest(request, map_id, GET_ALL_UPDATES);
                    do{
                        send(sockfd, request, sizeof(request), 0);
                        recv(sockfd, response, sizeof(response), 0);
                    }while(response[1] != RSP_OK);

                    //write get_all_update to log
                    logTxt[0] = '\0';
                    sprintf(logTxt, "[%d] GET_ALL_UPDATES: %d %d\n", map_id, response[1], response[2]);
                    fputs(logTxt,logfp);
                }
                close(sockfd);
                //write close to log
                logTxt[0] = '\0';
                sprintf(logTxt, "[%d] close connection\n", map_id);
                fputs(logTxt,logfp);
                break;
            case CHECKOUT:
                //Connecting to the server
                // Create a TCP socket.
                sockfd = socket(AF_INET , SOCK_STREAM , 0);
                // Specify an address to connect to (we use the local host or 'loop-back' address).
                address.sin_family = AF_INET;
                address.sin_port = htons(server_port);
                address.sin_addr.s_addr = inet_addr(server_ip); //for local host 127.0.01
                if(connect(sockfd, (struct sockaddr *) &address, sizeof(address)) == 0){
                    //writte connection to log
                    logTxt[0] = '\0';
                    sprintf(logTxt, "[%d] open connection\n", map_id);
                    fputs(logTxt,logfp);
                    setRequest(request, map_id, CHECKOUT);
                    send(sockfd, request, sizeof(request), 0);
                    recv(sockfd, response, sizeof(response), 0);
                    //write checkout to log
                    logTxt[0] = '\0';
                    sprintf(logTxt, "[%d] CHECKOUT: %d %d\n", map_id, response[1], response[2]);
                    fputs(logTxt,logfp);
                }
                close(sockfd);
                //write close to log
                logTxt[0] = '\0';
                sprintf(logTxt, "[%d] close connection\n", map_id);
                fputs(logTxt,logfp);
                break;
            default : 
                logTxt[0] = '\0';
                sprintf(logTxt, "[%d] wrong command\n", map_id);
                fputs(logTxt,logfp);
                break;
        }
    }
    return 0;
}



int main(int argc, char *argv[]) {
    int mappers;
    char folderName[100] = {'\0'};
    char *server_ip;
    int server_port;
    
    if (argc == 5) { // 4 arguments
        strcpy(folderName, argv[1]);
        mappers = atoi(argv[2]);
        server_ip = argv[3];
        server_port = atoi(argv[4]);
        if (mappers > MAX_MAPPER_PER_MASTER) {
            printf("Maximum number of mappers is %d.\n", MAX_MAPPER_PER_MASTER);
            printf("./client <Folder Name> <# of mappers> <server IP> <server Port>\n");
            exit(1);
        }

    } else {
        printf("Invalid or less number of arguments provided\n");
        printf("./client <Folder Name> <# of mappers> <server IP> <server Port>\n");
        exit(1);
    }

    // create log file
    createLogFile();

    // phase1 - File Path Partitioning
    traverseFS(mappers, folderName);

    // Phase2 - Mapper Clients's Deterministic Request Handling
    pid_t childpid;
    for (int i = 1; i < mappers + 1; i++){ //creating mappers
        // create the mapper file name
        char mFile[100]; //mapper file
        mFile[0] = '\0';
        sprintf(mFile, "MapperInput/Mapper_%d.txt", i);
        childpid = fork();
        if(!childpid){ //mapper code
            mapperCode(i, mFile, server_ip, server_port, logfp);
        }

    }

    while (r_wait(NULL) > 0); //wait for Mappers to finish
    

    // Phase3 - Master Client's Dynamic Request Handling (Extra Credit)
    masterCode(server_ip, server_port, logfp);

    fclose(logfp);
    return 0;

}