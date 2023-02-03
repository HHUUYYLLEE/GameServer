#include "clients.h"
#include "threads.h"


pthread_mutex_t roomListLock;
gameRoom *firstRoom = NULL, *lastRoom = NULL;
unsigned int roomID = 100;
gameRoom *createGameRoom(char *roomName){
	
	int i = 0;
	gameRoom *newRoom = malloc(sizeof(gameRoom));
	if(!newRoom){
		puts("Fail to create room");
		return NULL;
	}
	newRoom->roomID = roomID++
	strcpy(newRoom->roomName, roomName);
	for(i = 0; i < 4; ++i) newRoom->client_socket[i] = -1;
	for(i = 0; i < 52; ++i) newRoom->cards[i] = i;
	shuffle(newRoom->cards, 52);
	return newRoom;

}
void addRoomToList(char *roomName){
	pthread_mutex_lock(&roomListLock);
	gameRoom *newRoom = createGameRoom(roomName);
	if(!newRoom) {
		pthread_mutex_lock(&roomListLock);
		return;
	}
	if(!firstRoom){
		firstRoom = newRoom;
		lastRoom = firstRoom;
		pthread_mutex_unlock(&roomListLock);
		return;
	}
	lastRoom->next = newRoom;
	lastRoom = newRoom;
	pthread_mutex_unlock(&roomListLock);
}
void deleteThisRoom(unsigned int roomID){
	pthread_mutex_lock(&roomListLock);
	 // Store head node
    gameRoom *temp = firstRoom, *prev = NULL;
    // If head node itself holds the key to be deleted
    if (temp && temp->roomID == roomID) {
        firstRoom = temp->next; // Changed head
        free(temp); // free old head
		pthread_mutex_unlock(&roomListLock);
        return;
    }
 
    // Search for the key to be deleted, keep track of the
    // previous node as we need to change 'prev->next'
    while (temp && temp->roomID != roomID) {
        prev = temp;
        temp = temp->next;
    }
 
    // If key was not present in linked list
    if (!temp){
		pthread_mutex_unlock(&roomListLock);
        return;
	}

	if(temp == lastRoom) lastRoom = prev;
    // Unlink the node from linked list
    prev->next = temp->next;
    free(temp); // Free memory
	pthread_mutex_unlock(&roomListLock);
	
}
int insertClientToRoom(int client_socket, unsigned int roomID){
	pthread_mutex_lock(&roomListLock);
	gameRoom *temp = firstRoom;
	while(temp)
	{
		if(temp->roomID == roomID) {
			for (int i = 0; i < 4; ++i) if(temp->client_socket[i] == -1)
			{
				temp->client_socket[i] = client_socket;
				pthread_mutex_unlock(&roomListLock);
				return 1;
			}
			pthread_mutex_unlock(&roomListLock);
			return 0;
		}
		temp = temp->next;
	}

	pthread_mutex_unlock(&roomListLock);
	return 0;
}
void removeClientFromRoom(int client_socket, unsigned int roomID){
	pthread_mutex_lock(&roomListLock);
	gameRoom *temp = firstRoom;
	while(temp)
	{
		if(temp->roomID == roomID) {
			for (int i = 0; i < 4; ++i) if(temp->client_socket[i] == client_socket)
			{
				temp->client_socket[i] = -1;
				pthread_mutex_unlock(&roomListLock);
				return;
			}
			pthread_mutex_unlock(&roomListLock);
			return;
		}
		temp = temp->next;
	}

	pthread_mutex_unlock(&roomListLock);
	return 0;
}
void deleteAllRooms(){
	pthread_mutex_lock(&roomListLock);
	gameRoom *temp = NULL;
	while(firstRoom){
	temp = firstRoom;
	firstRoom = firstRoom->next;
	free(temp);
	}
	pthread_mutex_unlock(&roomListLock);
}
void *receiving_handler(void *client){
	int socket =  (*(clientStatus *) client).socket;
	pthread_mutex_t *lock = (*(clientStatus *) client).client_lock;
	char *message = (*(clientStatus *) client).message;
	bool *startReceiving = &(*(clientStatus *) client).startReceiving;
	bool *disconnected = &(*(clientStatus *) client).disconnected;
	while(1){
		if(*startReceiving == false) continue;
			pthread_mutex_lock(lock);
			memset(message, 0, BUFF_SIZE);
			if(customRecv(socket, message) == 0) 
			{
				*disconnected = true;
				*startReceiving = false;
				pthread_mutex_unlock(lock);
				return NULL;
			}
			*startReceiving = false;
			pthread_mutex_unlock(lock);
	}
	return NULL;
}
void *client_handler(void *thread){
	clientStatus client;
	client.disconnected = false;
	client.socket = ((threadList *) thread)->client_socket;
	client.client_lock = &((threadList *) thread)->client_lock;
	unsigned int ID = ((threadList *) thread)->ID;
	if( pthread_mutex_init(client.client_lock, NULL) != 0 ) {
		puts("Mutex init for client has failed");
		closeSocketAndDeleteThread(client.socket, ID, NULL);
		return NULL;
	}

	
	pthread_t receivingThread;
	client.startReceiving = true;
	if( pthread_create( &receivingThread, NULL ,  receiving_handler , &client) < 0){
		puts("Could not create receiving thread");
		closeSocketAndDeleteThread(client.socket, ID, client.client_lock);
		return NULL;
	}
	const int menuState = 1, playingState = 2;
	int sent_status, gameState = menuState;
	while(1)
	{
		if(client.startReceiving == true) continue;
		pthread_mutex_lock(client.client_lock);
		if(client.disconnected == true) {
			pthread_mutex_unlock(client.client_lock);
			closeSocketAndDeleteThread(client.socket, ID, client.client_lock);
			return NULL;
		}
		
		checkGameState:
		switch(gameState){

			case menuState:

			break;


			case playingState:

			break;




			default:
			goto checkGameState;
			break;
		}
		
		
		client.startReceiving = true;
		pthread_mutex_unlock(client.client_lock);
	}
	return NULL;
}

int customRecv(int socket, char *buff){
    bzero(buff, BUFF_SIZE);
    char length[6];
    int i = recv(socket, length, 5, 0);
    if(i == 0) return 0;
    int len = atoi(length);
    i = recv(socket, buff, len, 0);
    if(i == 0) return 0;
    return i;
}
int customSend(int socket, char *buff){
	int sentBytes;
    int len = strlen(buff);
    int times = 1;
    char zero = '0';
    char zeroString[6] = "";
    while(times * len < 10000){
        strncat(zeroString, &zero, 1);
        times *= 10;
    }
    char length[11];
    char temp[6];
    sprintf(temp, "%d", len);
    sprintf(length, "%s%s", zeroString, temp);
    if(send(socket, length, 5, 0) <= 0) return 0;
    if(send(socket, buff, len, 0) <= 0) return 0;
	return 1;
}
void shuffle(int *array, size_t n) {    
    if (n > 1) {
        size_t i;
        for (i = n - 1; i > 0; i--) {
            size_t j = (unsigned int) (drand48()*(i+1));
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}

int sendAndCheckError(int client_socket, char *message, unsigned int threadID, pthread_mutex_t *lock){
	if(customSend(socket, message) == 0){
		pthread_mutex_unlock(lock);
		closeSocketAndDeleteThread(client_socket, threadID, lock);
		return 0;
	}
	return 1;
}