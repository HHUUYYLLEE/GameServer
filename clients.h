
#ifndef _clients_h_
#define _clients_h_
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/socket.h>
#define BUFF_SIZE 1024
typedef struct clientStatus{
	int socket;
	bool startReceiving;
	char message[BUFF_SIZE];
	pthread_mutex_t *client_lock;
	bool disconnected;
} clientStatus;

typedef struct gameRoom{
	unsigned int roomID;
	char roomName[BUFF_SIZE];
	int client_socket[4];
	int cards[52];
	struct gameRoom *next;
} gameRoom;

extern pthread_mutex_t roomListLock;
extern gameRoom *firstRoom, *lastRoom;
extern unsigned int roomID;
gameRoom *createGameRoom(char *roomName);
void addRoomToList(char *roomName);
void deleteThisRoom(unsigned int ID);
void deleteAllRooms();
void insertClientToRoom(int client_socket, unsigned int roomID)
void removeClientFromRoom(int client_socket, unsigned int roomID);
void shuffle(int *array, size_t n);
void *client_handler(void *);
void *receiving_handler(void *);
int customRecv(int socket, char *buff);
int customSend(int socket, char *buff);
#endif