test machine: CSELAB_kh4250-15\
date: 12/02/2019\
name: Azul Bolaños, Adam Comerford\
x500: bolan108, comer059

Description:\
This project is an implementation of Map/Reduce through the Client/Server model communicating on the TCP/IP protocol family. There is a Master Client which spawns child processes (Mappers) to connect to the server, and a Multi-threaded Server (Reducer) to handle the requests of the mapper clients. The Master Client traverses a file directory, counts the first letters of each word within a text file, then forks Mapper Clients which connect to and send requests to the Server, which creates a new thread to handle the requests of each Mapper Client.\
The Server reduces the data from each Mapper and keeps track of information like total letter count and the number of updates for each client.\
The Application Protocol has 6 commands:
- CHECKIN - checks a client into the server
- UPDATE_AZLIST - adds the letter count sent from client to total letter count, azList
- GET_AZLIST - sends the current letter count back to client
- GET_MAPPER_UPDATES - tells client how many times it has updates azList
- GET_ALL_UPDATES - tells client how many times azList has been updated by all clients
- CHECKOUT - checks client out of server. Done before connection breaks.

Extra-Credit:\
The program properly handles the master client's dynamic request handling. After each mapper process is terminated, it can make requests (3) GET_AZLIST and (5) GET_ALL_UPDATES.

Compilation:\
Both Client and Server files contain a makefile which compiles the program. Simply run:\
```make```\
In the client and servers' respective directories.
To run the Client enter:\
```./client <folder name> <# of mappers> <server IP> <server port>```\
To run the Server enter:\
```./server <server port>```\
Where:
- < folder name> is the name of the folder to be traversed
- <# of mappers> is a specified number of mappers
- < server IP> is the IP address of the server
- < server port> is the port number of the server

Assumptions:
- The server remains listening after all clients have finished so the server port remains open.
- When run on the KH-250 lab machines, a strange file with a random integer name appears in the directory. We cannot open it nor remove it. It does not appear on our personal machines, only on the Unix machines in the lab. We assume this is inconsequential.

Contributions:\
We worked separately on the two sides of the Client/Server. Each individual's work can be seen by examining the process they worked on. Both involved in planning/preperation.
- Azul Bolaños: Client process, Extra Credit (Master Client dynamic requests)
- Adam Comerford: Server process, README
