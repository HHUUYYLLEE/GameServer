#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include <netinet/in.h>

#include <ctype.h>

#include "clients.h"

//Remember to use -pthread when compiling this server's source code




int main()
{
	struct timeval tv;
    gettimeofday(&tv, NULL);
    int usec = tv.tv_usec;
    srand48(usec);

	printf("\033[0;31m"); //print red text 

	if (pthread_mutex_init(&lock1, NULL) != 0 || pthread_mutex_init(&lock2, NULL) != 0  || 
		pthread_mutex_init(&roomListLock, NULL) != 0 || pthread_mutex_init(&dbLock, NULL) != 0 ||
		pthread_mutex_init(&clientStatusListLock, NULL) != 0)
		{
			
        puts("Mutex init has failed");
        exit(EXIT_FAILURE);
    	}

	mysqlPtr = mysql_init(NULL);
	 if (!mysqlPtr)
  	{
      fprintf(stderr, "%s\n", mysql_error(mysqlPtr));
      exit(1);
  	}

  	if (mysql_real_connect(mysqlPtr, "localhost", "huy", "123456",
          "baitienlen", 0, NULL, 0) == NULL)
  	{
      fprintf(stderr, "%s\n", mysql_error(mysqlPtr));
	  mysql_close(mysqlPtr);
      exit(1);
  	}

	int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket==-1){
	perror("Socket initialisation failed");
	exit(EXIT_FAILURE);
	}
    else
	puts("Server socket created successfully");

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8000);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	int client_socket;
    //bind the socket to the specified IP addr and port
    if (bind(server_socket, (struct sockaddr*) &server_addr, sizeof(server_addr))!=0){
		perror("ERROR on binding");
        exit(EXIT_FAILURE);
	}
    else
	puts("Socket successfully binded.."); 
    
    if (listen(server_socket, 3)!=0){
		puts("Listen failed..."); 
        exit(EXIT_FAILURE); 
    } 
    else
        puts("Server listening.."); 
    printf("\033[0m"); //default color
	bool threadCreationSuccess = false;
    while (1){
	client_socket = accept(server_socket, NULL, NULL);
	if (client_socket < 0) { 
        	puts("Server acccept failed..."); 
			continue;
    } 
	createThreadClient(client_socket, &threadCreationSuccess);
	if(threadCreationSuccess == false) break;
	
	}
	puts("call joining thread function");
	pthread_mutex_lock(&lock2);
	threadList *temp = firstThreadSlot;
    while(temp) {
		pthread_join(temp->thread, NULL);
		temp = temp->next;
	}
	pthread_mutex_unlock(&lock2);
	
    close(server_socket);
	pthread_mutex_lock(&lock2);
    deleteAllThreads();
	pthread_mutex_unlock(&lock2);

	pthread_mutex_destroy(&lock1);
	pthread_mutex_destroy(&lock2);
	pthread_mutex_destroy(&roomListLock);
	pthread_mutex_destroy(&dbLock);
	pthread_mutex_destroy(&clientStatusListLock);
	mysql_close(mysqlPtr);
    return 0;
}

