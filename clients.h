#ifndef _clients_h_
#define _clients_h_
#include "database.h"
#include "threads.h"
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#define BUFF_SIZE 1024

#define menuState 1
#define player1State 2
#define roomOwnerState 3
#define roomGuestState 4
#define player2State 5
#define loginCode 1
#define passwordCode 2
#define roomSelectCode 3
#define playCardsCode 4
#define signUpCode 5
#define otherCode 6
#define createNewRoomCode 7

typedef struct gameRoom{
	unsigned int roomID;
	char roomName[BUFF_SIZE];
	int cards[52];
	int client_count;
	bool player1Turn;
	bool player2Turn;
	bool player1Quit;
	bool player2Quit;
	bool player1Win;
	bool player2Win;
	int messageCode1;
	int messageCode2;
	char message1[BUFF_SIZE];
	char message2[BUFF_SIZE];
	bool gameStarted;
	struct gameRoom *next;
} gameRoom;


unsigned int getThreadID(int socket);
pthread_mutex_t roomListLock, clientStatusListLock;
gameRoom *firstRoom, *lastRoom;
unsigned int roomID;
gameRoom *createGameRoom(char *roomName);
int addRoomToList(unsigned int *roomID, char *roomName);
void deleteThisRoom(unsigned int roomID);
void deleteAllRooms();
void setupGame(unsigned int roomID);
int makeRoomStartGame(unsigned int roomID);
void getPlayer1Cards(unsigned int roomID, char *message);
void getPlayer2Cards(unsigned int roomID, char *message);
bool ifPlayer1Quit(unsigned int roomID);
bool ifPlayer2Quit(unsigned int roomID);
void player1GoesFirst(unsigned int roomID);
void player2GoesFirst(unsigned int roomID);
int getClientCount(unsigned int roomID);
void removeClientFromRoom(unsigned int roomID);
int insertClientToRoom(unsigned int *roomID, char *roomName);
void removeClientFromRoom(unsigned int roomID);
bool checkRoomGameStarted(unsigned int roomID);
bool checkPlayer1Turn(unsigned int roomID, int *messageCode, char *message);
bool checkPlayer2Turn(unsigned int roomID, int *messageCode, char *message);
void player1Quit(unsigned int roomID);
void player2Quit(unsigned int roomID);
void player1Win(unsigned int roomID);
void player2Win(unsigned int roomID);
bool ifPlayer1Win(unsigned int roomID);
bool ifPlayer2Win(unsigned int roomID);
void player1Play(unsigned int roomID, int messageCode, char *message);
void player2Play(unsigned int roomID, int messageCode, char *message);
void player1EndTurn(unsigned int roomID);
void player2EndTurn(unsigned int roomID);
void shuffle(int *array, size_t n);
void *client_handler(void *);
int customRecv(int socket, int *messageCode, char *buff);
int customSend(int socket, int messageCode, char *buff);
bool sendSuccess(int client_socket, int messageCode, char *message, unsigned int threadID);
bool recvSuccess(int client_socket, int *messageCode, char *message, unsigned int threadID);
#endif