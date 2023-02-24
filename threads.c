#include "threads.h"

threadList *firstThreadSlot = NULL, *lastThreadSlot = NULL;
pthread_mutex_t lock1, lock2;
unsigned int threadID = 100;
threadList *createThreadSlot(int client_socket){
	threadList *new = malloc(sizeof(threadList));
	if(!new) return NULL;
	new->next = NULL;
	new->client_socket = client_socket;
	new->ID = threadID++;
	return new;
}
void insertThisThread(threadList *new){
	pthread_mutex_lock(&lock2);
	if(!firstThreadSlot){
		firstThreadSlot = new;
		lastThreadSlot = firstThreadSlot;
		pthread_mutex_unlock(&lock2);
		return;
	}
	lastThreadSlot->next = new;
	lastThreadSlot = new;
	pthread_mutex_unlock(&lock2);
}
void deleteThisThread(unsigned int ID){
	
	pthread_mutex_lock(&lock2);
	 // Store head node
    threadList *temp = firstThreadSlot, *prev = NULL;
    // If head node itself holds the key to be deleted
    if (temp && temp->ID == ID) {
        firstThreadSlot = temp->next; // Changed head
        free(temp); // free old head
		pthread_mutex_unlock(&lock2);
        return;
    }
 
    // Search for the key to be deleted, keep track of the
    // previous node as we need to change 'prev->next'
    while (temp && temp->ID != ID) {
        prev = temp;
        temp = temp->next;
    }
 
    // If key was not present in linked list
    if (!temp){
		pthread_mutex_unlock(&lock2);
        return;
	}

	if(temp == lastThreadSlot) lastThreadSlot = prev;
    // Unlink the node from linked list
    prev->next = temp->next;
    free(temp); // Free memory
	pthread_mutex_unlock(&lock2);

	
}
void deleteAllThreads(){
	pthread_mutex_lock(&lock2);
	threadList *temp = NULL;
	while(firstThreadSlot){
	temp = firstThreadSlot;
	firstThreadSlot = firstThreadSlot->next;
	free(temp);
	}
	pthread_mutex_unlock(&lock2);
}
void createThreadClient(int client_socket, bool *threadCreationSuccess){
	threadList *new = createThreadSlot(client_socket);
	if(!new){
		puts("Error allocating memory for thread");
		close(client_socket);
		*threadCreationSuccess = false;
		return;
	}

	if( pthread_create( &new->thread, NULL ,  client_handler , new ) < 0){
		puts("Could not create thread");
		close(client_socket);
		*threadCreationSuccess = false;
		return;
	}

	
	insertThisThread(new);
	
	*threadCreationSuccess = true;
	puts("Server acccept the client...");
	
}

void closeSocketAndDeleteThread(int client_socket, unsigned int ID){
	close(client_socket);
	deleteThisThread(ID);
}