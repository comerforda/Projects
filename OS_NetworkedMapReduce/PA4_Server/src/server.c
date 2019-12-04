#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
//#include <zconf.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <arpa/inet.h>
#include <errno.h>
#include "../include/protocol.h"

pthread_mutex_t azList_lock[ALPHABETSIZE];
pthread_mutex_t US_lock;
pthread_mutex_t thread_lock;

int azList[ALPHABETSIZE];
int updateStatus[MAX_STATUS_TABLE_LINES][3]; //max 50 connections. mapper id = 0, # updates = 1, check in/out = 2
sem_t tsem;

struct clientInfo
{
    int clientfd;
    char *clientip;
    int clientport;
};

/* Function each thread is called on. Handles and responds to every request of a respective client
   until the client has been checked out and breaks the connection */
void *threadResponse(void *arg)
{
    struct clientInfo *client = (struct clientInfo *)arg;
    int request[REQUEST_MSG_SIZE];            //client request data
    int response[RESPONSE_MSG_SIZE];          //Response data (size 3)
    int longResponse[LONG_RESPONSE_MSG_SIZE]; //For GET_AZLIST only!! (size 28)
    int mapperID;
    int command; //request command
    int i;       //loop counter

    //Client request DISPATCHER
    while (1)
    {
        //init response buffers
        memset(response, 0, RESPONSE_MSG_SIZE * sizeof(int));
        memset(longResponse, 0, LONG_RESPONSE_MSG_SIZE * sizeof(int));
        //read data from client
        read(client->clientfd, request, sizeof(request));
        command = request[0];
        mapperID = request[1];
        response[0] = command;

        if (command == CHECKIN)
        {
            printf("[%d] CHECKIN\n", mapperID);
            pthread_mutex_lock(&US_lock);

            if (mapperID > 0 && updateStatus[mapperID - 1][0] != 0) //entry exists
            {
                updateStatus[mapperID - 1][2] = 1; //check in mapper
                response[1] = RSP_OK;              //success
            }
            else if (mapperID > 0) //entry doesn't exist
            {
                updateStatus[mapperID - 1][0] = mapperID;
                updateStatus[mapperID - 1][2] = 1;
                response[1] = RSP_OK; //success
            }
            else
            {
                fprintf(stderr, "ERROR! Invalid mapper id for checkin: %d\n", mapperID);
                response[1] = RSP_NOK;
            }

            pthread_mutex_unlock(&US_lock);
            response[2] = mapperID;                              //response data
            write(client->clientfd, response, sizeof(response)); //respond to client
            if(mapperID == MASTER_ID)
                break;
        }

        //Adds client's data to azList
        else if (command == UPDATE_AZLIST)
        {
            //Update one element of azList at a time. Each has an individual lock for thread concurrency.
            if (mapperID == MASTER_ID){
                response[1] = RSP_NOK;
                break;
            }
            for (i = 2; i < 28; ++i)
            {
                pthread_mutex_lock(&azList_lock[i - 2]);
                azList[i - 2] += request[i];
                pthread_mutex_unlock(&azList_lock[i - 2]);
            }
            //increment update field of updateStatus
            pthread_mutex_lock(&US_lock);
            updateStatus[mapperID - 1][1] += 1;
            pthread_mutex_unlock(&US_lock);

            response[1] = RSP_OK; //update success
            response[2] = mapperID;
            write(client->clientfd, response, sizeof(response)); //respond to client
        }

        //Sends current azList to client
        else if (command == GET_AZLIST)
        {
            printf("[%d] GET_AZLIST\n", mapperID);
            for (i = 2; i < LONG_RESPONSE_MSG_SIZE; ++i)
            {
                pthread_mutex_lock(&azList_lock[i - 2]);
                longResponse[i] = azList[i - 2];
                pthread_mutex_unlock(&azList_lock[i - 2]);
            }
            longResponse[1] = RSP_OK;
            write(client->clientfd, longResponse, sizeof(longResponse));
            if(mapperID == MASTER_ID)
                break;
        }

        //Sends # of updates of client
        else if (command == GET_MAPPER_UPDATES)
        {
            printf("[%d] GET_MAPPER_UPDATES\n", mapperID);
            if (mapperID == MASTER_ID){
                response[1] = RSP_NOK;
                write(client->clientfd, response, sizeof(response)); //respond to master
                break;
            }
            pthread_mutex_lock(&US_lock);
            response[2] = updateStatus[mapperID - 1][1];
            pthread_mutex_unlock(&US_lock);

            response[1] = RSP_OK;
            write(client->clientfd, response, sizeof(response)); //respond to client
        }

        //Sends # of updates of ALL clients
        else if (command == GET_ALL_UPDATES)
        {
            int sum = 0;
            printf("[%d] GET_ALL_UPDATES\n", mapperID);

            pthread_mutex_lock(&US_lock);
            for (i = 0; i < MAX_STATUS_TABLE_LINES; i++)
                sum += updateStatus[i][1];
            pthread_mutex_unlock(&US_lock);

            response[1] = RSP_OK;
            response[2] = sum;
            write(client->clientfd, response, sizeof(response));
            if(mapperID == MASTER_ID)
                break;
        }

        else if (command == CHECKOUT)
        {
            printf("[%d] CHECKOUT\n", mapperID);
            if(mapperID == MASTER_ID){
                response[1] = RSP_NOK;
                response[2] = mapperID;
                write(client->clientfd, response, sizeof(response)); //respond to master
                break;
            }
            response[0] = CHECKOUT;
            response[2] = mapperID;

            if (updateStatus[mapperID - 1][2] != 1)
            {
                fprintf(stderr, "ERROR! Mapper: %d is not checked in.\n", mapperID);
                response[1] = RSP_NOK; //CHECKOUT fail
                write(client->clientfd, response, sizeof(response));
            }
            else
            {
                pthread_mutex_lock(&US_lock);
                updateStatus[mapperID - 1][2] = 0; //set checkout
                pthread_mutex_unlock(&US_lock);
                response[1] = RSP_OK;                                //CHECKOUT success
                write(client->clientfd, response, sizeof(response)); //respond to client
                break;  //EXIT CONDITION: client is checked out, break loop and close connection
            }
        }

        else
            fprintf(stderr, "Invalid request code: %d entered.\n\n", command);
    }
    printf("close connection from %s:%d\n", client->clientip, client->clientport);
    close(client->clientfd);
    sem_post(&tsem);
    return NULL;
}

int main(int argc, char *argv[])
{
    int server_port;

    if (argc == 2)
    { // 1 arguments
        server_port = atoi(argv[1]);
    }
    else
    {
        printf("Invalid or less number of arguments provided\n");
        printf("./server <server Port>\n");
        exit(0);
    }

    pthread_mutex_init(&US_lock, NULL);         //init Status lock
    sem_init(&tsem, 0, MAX_CONCURRENT_CLIENTS); //init max thread semaphore

    //init azList lock
    for (int i = 0; i < ALPHABETSIZE; i++)
        pthread_mutex_init(&azList_lock[i], NULL);

    //init updateStatus table to 0
    for (int i = 0; i < MAX_STATUS_TABLE_LINES; i++)
    {
        for (int j = 0; j < 3; j++)
            updateStatus[i][j] = 0;
    }

    //Create TCP socket. AF_INET = TCP/IP protocol fam, SOCK_STREAM specifies TCP
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    //Bind our socket to a local address.
    struct sockaddr_in servAddress;
    servAddress.sin_family = AF_INET;                //TCP/IP protocol
    servAddress.sin_port = htons(server_port);       //Our port number
    servAddress.sin_addr.s_addr = htonl(INADDR_ANY); //Accept any IP
    if (bind(sock, (struct sockaddr *)&servAddress, sizeof(servAddress)) == -1)
        perror("Failed to bind the socket to port");

    //Listen on our bound port
    if (listen(sock, MAX_CONCURRENT_CLIENTS) == -1)
        perror("Failed to listen on port");
    printf("server is listening\n");

    // Server (Reducer) code
    while (1)
    {
        //Accept incoming connections. 'accept' initialized clientAddress.
        struct sockaddr_in clientAddress;
        socklen_t addrlen = sizeof(struct sockaddr_in);

        sem_wait(&tsem); //Only accept client if < 50 concurrent connections
        int clientfd = accept(sock, (struct sockaddr *)&clientAddress, &addrlen);

        //initialize our new clientInfo structure to accepted clientAddress.
        struct clientInfo *client = (struct clientInfo *)malloc(sizeof(struct clientInfo));
        client->clientfd = clientfd;
        client->clientip = inet_ntoa(clientAddress.sin_addr);
        client->clientport = clientAddress.sin_port;
        printf("open connection from %s:%d\n", client->clientip, client->clientport);

        //Client-Server communication is set. Now handle requests.

        pthread_t thread;
        pthread_create(&thread, NULL, threadResponse, (void *)client);
        pthread_detach(thread);
    }

    close(sock); //Close server
}