#include "clients.h"
#include "threads.h"
#include "deque.h"

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
	for (i = 0; i < 52; ++i) newRoom->cards[i] = i;
	newRoom->roomID = roomID++;
	strcpy(newRoom->roomName, roomName);
	newRoom->gameStarted = false;
	newRoom->client_count = 0;
	newRoom->player1Turn = false;
	newRoom->player2Turn = false;
	newRoom->player1Quit = false;
	newRoom->player2Quit = false;
	newRoom->player1Win = false;
	newRoom->player2Win = false;
	newRoom->messageCode1 = 0;
	newRoom->messageCode2 = 0;
	strcpy(newRoom->message1, "temp");
	strcpy(newRoom->message2, "temp");
	newRoom->gameStarted = false;
	return newRoom;

}

int makeRoomStartGame(unsigned int roomID){
	pthread_mutex_lock(&roomListLock);
	gameRoom *temp = firstRoom;
	while(temp)
	{
		if(temp->roomID == roomID) {
			if(temp->client_count == 2)
			{
				temp->gameStarted = true;
				pthread_mutex_unlock(&roomListLock);
				return 0;
			}
			pthread_mutex_unlock(&roomListLock);
			return 1;
		}
		temp = temp->next;
	}

	pthread_mutex_unlock(&roomListLock);
	return 1;
}

int addRoomToList(unsigned int *roomID, char *roomName){
	pthread_mutex_lock(&roomListLock);
	gameRoom *newRoom = createGameRoom(roomName);
	if(!newRoom) {
		pthread_mutex_lock(&roomListLock);
		return 0;
	}
	if(!firstRoom){
		firstRoom = newRoom;
		lastRoom = firstRoom;
		*roomID = newRoom->roomID;
		pthread_mutex_unlock(&roomListLock);
		return 0;
	}
	gameRoom *temp = firstRoom;
	while(temp){
		if(strcmp(temp->roomName,roomName) == 0) {
			pthread_mutex_unlock(&roomListLock);
			return 1;
		}
		temp = temp->next;
	}
	lastRoom->next = newRoom;
	lastRoom = newRoom;
	*roomID = newRoom->roomID;
	pthread_mutex_unlock(&roomListLock);
	return 0;
}

void deleteThisRoom(unsigned int roomID){
	pthread_mutex_lock(&roomListLock);

    gameRoom *temp = firstRoom, *prev = NULL;
  
    if (temp && temp->roomID == roomID) {
		if(temp->client_count != 0){
			pthread_mutex_unlock(&roomListLock);
        	return;
		}
        firstRoom = temp->next;
        free(temp); 
		pthread_mutex_unlock(&roomListLock);
        return;
    }
 
    
    while (temp && temp->roomID != roomID) {
        prev = temp;
        temp = temp->next;
    }
 
    
    if (!temp){
		pthread_mutex_unlock(&roomListLock);
        return;
	}

	if(temp == lastRoom) lastRoom = prev;
    
    prev->next = temp->next;
	if(temp->client_count != 0){
			pthread_mutex_unlock(&roomListLock);
        	return;
	}
    free(temp);
	pthread_mutex_unlock(&roomListLock);
	
}
int insertClientToRoom(unsigned int *roomID, char *roomName){
	pthread_mutex_lock(&roomListLock);
	gameRoom *temp = firstRoom;
	while(temp)
	{
		if(strcmp(roomName, temp->roomName) == 0) {
			if(temp->client_count < 2)
			{
				*roomID = temp->roomID;
				temp->client_count++;
				pthread_mutex_unlock(&roomListLock);
				return 0;
			}
			else
			{
				pthread_mutex_unlock(&roomListLock);
				//can't add
				return 1;
			}
		}
		temp = temp->next;
	}

	pthread_mutex_unlock(&roomListLock);
	// can't add
	return 1;
}
int getClientCount(unsigned int roomID){
	pthread_mutex_lock(&roomListLock);
	gameRoom *temp = firstRoom;
	int count;
	while(temp)
	{
		if(temp->roomID == roomID) 
			{		
				count = temp->client_count;
				pthread_mutex_unlock(&roomListLock);
				return count;
			}
			temp = temp->next;
	}
	pthread_mutex_unlock(&roomListLock);
	return 99;
}
void removeClientFromRoom(unsigned int roomID){
	pthread_mutex_lock(&roomListLock);
	gameRoom *temp = firstRoom;
	while(temp)
	{
		if(temp->roomID == roomID) 
			{		
				temp->client_count--;
				pthread_mutex_unlock(&roomListLock);
				return;
			}
			temp = temp->next;
	}
	pthread_mutex_unlock(&roomListLock);
	return;
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
void setupGame(unsigned int roomID){
	pthread_mutex_lock(&roomListLock);
	gameRoom *temp = firstRoom;
	while(temp)
	{
		if(temp->roomID == roomID) {
				shuffle(temp->cards, 52);
				strcpy(temp->message1, "temp");
				strcpy(temp->message2, "temp");
				pthread_mutex_unlock(&roomListLock);
				return;
		}
		temp = temp->next;
	}
	pthread_mutex_unlock(&roomListLock);
	return;
}
void getPlayer1Cards(unsigned int roomID, char *message){
	pthread_mutex_lock(&roomListLock);
	gameRoom *temp = firstRoom;
	int i;
	char cardString[3];
	while(temp)
	{
		if(temp->roomID == roomID) {
				strcpy(message, "");
				for (i = 0; i < 26; ++i){
					if(temp->cards[i] < 10) sprintf(cardString, "0%d", temp->cards[i]);
					else sprintf(cardString, "%d",temp->cards[i]);
					strcat(message, cardString);
				}
				pthread_mutex_unlock(&roomListLock);
				return;
		}
		temp = temp->next;
	}
	pthread_mutex_unlock(&roomListLock);
	return;
}
void getPlayer2Cards(unsigned int roomID, char *message){
	pthread_mutex_lock(&roomListLock);
	gameRoom *temp = firstRoom;
	int i;
	char cardString[3];
	while(temp)
	{
		if(temp->roomID == roomID) {
				strcpy(message, "");
				for (i = 26; i < 52; ++i){
					if(temp->cards[i] < 10) sprintf(cardString, "0%d", temp->cards[i]);
					else sprintf(cardString, "%d", temp->cards[i]);
					strcat(message, cardString);
				}
				pthread_mutex_unlock(&roomListLock);
				return;
		}
		temp = temp->next;
	}
	pthread_mutex_unlock(&roomListLock);
	return;
}
void player1GoesFirst(unsigned int roomID){
	pthread_mutex_lock(&roomListLock);
	gameRoom *temp = firstRoom;
	while(temp)
	{
		if(temp->roomID == roomID) {
				temp->player1Turn = true;
				temp->player2Turn = false;
				pthread_mutex_unlock(&roomListLock);
				return;
		}
		temp = temp->next;
	}
	pthread_mutex_unlock(&roomListLock);
	return;
}
void player2GoesFirst(unsigned int roomID){
	pthread_mutex_lock(&roomListLock);
	gameRoom *temp = firstRoom;
	while(temp)
	{
		if(temp->roomID == roomID) {
				temp->player2Turn = true;
				temp->player1Turn = false;
				pthread_mutex_unlock(&roomListLock);
				return;
		}
		temp = temp->next;
	}
	pthread_mutex_unlock(&roomListLock);
	return;
}
bool checkRoomGameStarted(unsigned int roomID){
	pthread_mutex_lock(&roomListLock);
	gameRoom *temp = firstRoom;
	while(temp)
	{
		if(temp->roomID == roomID) {
			if(temp->gameStarted == true)
			{
				pthread_mutex_unlock(&roomListLock);
				return true;
			}
			pthread_mutex_unlock(&roomListLock);
			return false;
		}
		temp = temp->next;
	}
	pthread_mutex_unlock(&roomListLock);
	return false;
}
bool checkPlayer1Turn(unsigned int roomID, int *messageCode, char *message){
	pthread_mutex_lock(&roomListLock);
	gameRoom *temp = firstRoom;
	while(temp)
	{
		if(temp->roomID == roomID) {
			if(temp-> player1Turn == true)
			{
				*messageCode = temp->messageCode2;
				strcpy(message, temp->message2);
				pthread_mutex_unlock(&roomListLock);
				return true;
			}
			pthread_mutex_unlock(&roomListLock);
			return false;
		}
		temp = temp->next;
	}
	pthread_mutex_unlock(&roomListLock);
	return false;
}
bool checkPlayer2Turn(unsigned int roomID, int *messageCode, char *message){
	pthread_mutex_lock(&roomListLock);
	gameRoom *temp = firstRoom;
	while(temp)
	{
		if(temp->roomID == roomID) {
			if(temp-> player2Turn == true)
			{
				*messageCode = temp->messageCode1;
				strcpy(message, temp->message1);
				pthread_mutex_unlock(&roomListLock);
				return true;
			}
			pthread_mutex_unlock(&roomListLock);
			return false;
		}
		temp = temp->next;
	}
	pthread_mutex_unlock(&roomListLock);
	return false;
}
void player1Quit(unsigned int roomID){
	pthread_mutex_lock(&roomListLock);
	gameRoom *temp = firstRoom;
	while(temp)
	{
		if(temp->roomID == roomID) {
				temp->player1Quit = true;
				pthread_mutex_unlock(&roomListLock);
				return;
		}
		temp = temp->next;
	}
	pthread_mutex_unlock(&roomListLock);
	return;
}
void player2Quit(unsigned int roomID){
	pthread_mutex_lock(&roomListLock);
	gameRoom *temp = firstRoom;
	while(temp)
	{
		if(temp->roomID == roomID) {
				temp->player2Quit = true;
				pthread_mutex_unlock(&roomListLock);
				return;
		}
		temp = temp->next;
	}
	pthread_mutex_unlock(&roomListLock);
	return;
}
void player1Win(unsigned int roomID){
	pthread_mutex_lock(&roomListLock);
	gameRoom *temp = firstRoom;
	while(temp)
	{
		if(temp->roomID == roomID) {
				temp->player1Win = true;
				pthread_mutex_unlock(&roomListLock);
				return;
		}
		temp = temp->next;
	}
	pthread_mutex_unlock(&roomListLock);
	return;
}
void player2Win(unsigned int roomID){
	pthread_mutex_lock(&roomListLock);
	gameRoom *temp = firstRoom;
	while(temp)
	{
		if(temp->roomID == roomID) {
				temp->player2Win = true;
				pthread_mutex_unlock(&roomListLock);
				return;
		}
		temp = temp->next;
	}
	pthread_mutex_unlock(&roomListLock);
	return;
}
bool ifPlayer1Win(unsigned int roomID){
	pthread_mutex_lock(&roomListLock);
	gameRoom *temp = firstRoom;
	while(temp)
	{
		if(temp->roomID == roomID) {
			if(temp-> player1Win == true)
			{
				pthread_mutex_unlock(&roomListLock);
				return true;
			}
			pthread_mutex_unlock(&roomListLock);
			return false;
		}
		temp = temp->next;
	}
	pthread_mutex_unlock(&roomListLock);
	return false;
}
bool ifPlayer2Win(unsigned int roomID){
	pthread_mutex_lock(&roomListLock);
	gameRoom *temp = firstRoom;
	while(temp)
	{
		if(temp->roomID == roomID) {
			if(temp-> player2Win == true)
			{
				pthread_mutex_unlock(&roomListLock);
				return true;
			}
			pthread_mutex_unlock(&roomListLock);
			return false;
		}
		temp = temp->next;
	}
	pthread_mutex_unlock(&roomListLock);
	return false;
}
void player1Play(unsigned int roomID, int messageCode, char *message){
	pthread_mutex_lock(&roomListLock);
	gameRoom *temp = firstRoom;
	while(temp)
	{
		if(temp->roomID == roomID) {
				temp->messageCode1 = messageCode;
				strcpy(temp->message1, message);
				pthread_mutex_unlock(&roomListLock);
				return;
		}
		temp = temp->next;
	}
	pthread_mutex_unlock(&roomListLock);
	return;
}
void player2Play(unsigned int roomID, int messageCode, char *message){
	pthread_mutex_lock(&roomListLock);
	gameRoom *temp = firstRoom;
	while(temp)
	{
		if(temp->roomID == roomID) {
				temp->messageCode2 = messageCode;
				strcpy(temp->message2, message);
				pthread_mutex_unlock(&roomListLock);
				return;
		}
		temp = temp->next;
	}
	pthread_mutex_unlock(&roomListLock);
	return;
}
void player1EndTurn(unsigned int roomID){
	pthread_mutex_lock(&roomListLock);
	gameRoom *temp = firstRoom;
	while(temp)
	{
		if(temp->roomID == roomID) {
				temp->player1Turn = false;
				temp->player2Turn = true;
				pthread_mutex_unlock(&roomListLock);
				return;
		}
		temp = temp->next;
	}
	pthread_mutex_unlock(&roomListLock);
	return;
}
void player2EndTurn(unsigned int roomID){
	pthread_mutex_lock(&roomListLock);
	gameRoom *temp = firstRoom;
	while(temp)
	{
		if(temp->roomID == roomID) {
				temp->player2Turn = false;
				temp->player1Turn = true;
				pthread_mutex_unlock(&roomListLock);
				return;
		}
		temp = temp->next;
	}
	pthread_mutex_unlock(&roomListLock);
	return;
}

bool ifPlayer1Quit(unsigned int roomID){
	pthread_mutex_lock(&roomListLock);
	gameRoom *temp = firstRoom;
	while(temp)
	{
		if(temp->roomID == roomID) {
			if(temp-> player1Quit == true)
			{
				pthread_mutex_unlock(&roomListLock);
				return true;
			}
			pthread_mutex_unlock(&roomListLock);
			return false;
		}
		temp = temp->next;
	}
	pthread_mutex_unlock(&roomListLock);
	return false;
}

bool ifPlayer2Quit(unsigned int roomID){
	pthread_mutex_lock(&roomListLock);
	gameRoom *temp = firstRoom;
	while(temp)
	{
		if(temp->roomID == roomID) {
			if(temp-> player2Quit == true)
			{
				pthread_mutex_unlock(&roomListLock);
				return true;
			}
			pthread_mutex_unlock(&roomListLock);
			return false;
		}
		temp = temp->next;
	}
	pthread_mutex_unlock(&roomListLock);
	return false;
}


void *client_handler(void *thread){
	int socket = ((threadList *) thread)->client_socket;
	unsigned int ID = ((threadList *) thread)->ID, roomID;
	int messageCode;
	char message[BUFF_SIZE], username[BUFF_SIZE], password[BUFF_SIZE], roomName[BUFF_SIZE];
	bool loggedIn = false;
	int gameState = menuState;
	bool firstPlay;
	while(1)
	{
		strcpy(message, "");
		switch(gameState){
			case menuState:
			if( recvSuccess(socket, &messageCode, message, ID) == false ) goto disconnect;
				if(messageCode == loginCode){
					if(loggedIn == true) {
						if( logoutClient(mysqlPtr, username) == 1 ) puts("database error logging out");
					}
					strcpy(username, message);
					if( recvSuccess(socket, &messageCode, message, ID) == false ) goto disconnect;
					//password
					strcpy(password, message);
					switch( loginClient(mysqlPtr, username, password) ){
						case 1:
							//username doesn't exist
							if ( sendSuccess(socket, 0, "02", ID) == false ) goto disconnect;
						break;
						case 2:
							//wrongPassword
							if ( sendSuccess(socket, 0, "01", ID) == false ) goto disconnect;
						break;
						case 3:
							//loggedInSomeWhere
							if ( sendSuccess(socket, 0, "03", ID) == false ) goto disconnect;
						break;
						case 4:
							//database error
							if ( sendSuccess(socket, 0, "04", ID) == false ) goto disconnect;
						break;
						default:
						// success message
							if ( sendSuccess(socket, 1, "02", ID) == false ) goto disconnect;
							loggedIn = true;
						break;
					}
					//
				}

				if(messageCode == signUpCode){
					if(loggedIn == true) {
						if( logoutClient(mysqlPtr, username) == 1 ) puts("database error logging out");
					}
					strcpy(username, message);
					if( recvSuccess(socket, &messageCode, message, ID) == false ) goto disconnect;
					//password
					strcpy(password, message);
					if( createNewClient(mysqlPtr, username, password, false) == 1){
						//username existed
						if ( sendSuccess(socket, 0, "00", ID) == false ) goto disconnect;
						break;
					}
					// success message
					if ( sendSuccess(socket, 1, "02", ID) == false ) goto disconnect;
				}

				if(messageCode == createNewRoomCode){
					strcpy(roomName, message);
					if(addRoomToList(&roomID, roomName) == 1){
						//can't create room
						if ( sendSuccess(socket, 0, "05", ID) == false ) goto disconnect;
						break;
					}
					// success message
					if ( sendSuccess(socket, 1, "02", ID) == false ) goto disconnect;
					insertClientToRoom(&roomID, roomName);
					gameState = roomOwnerState;
				}

				if(messageCode == otherCode){
					if(strcmp(message, "07") == 0){
					//logout
						if(loggedIn == true) {
						if( logoutClient(mysqlPtr, username) == 1 ) puts("database error logging out");
					}
					}
				}

				if(messageCode == roomSelectCode){
					strcpy(roomName, message);
					if( insertClientToRoom(&roomID, roomName) == 1 )
					{
						puts("join failed");
						//can't join room
						if( sendSuccess(socket, 0, "07", ID) == false ) goto disconnect;
					}
					// success message
					else {
						puts("join success");
						if( sendSuccess(socket, 1, "02", ID) == false ) goto disconnect;
						gameState = roomGuestState;
					}
				}
				
			break;

			case roomOwnerState:
			if( recvSuccess(socket, &messageCode, message, ID) == false ) goto disconnect;
			if( messageCode == otherCode ){
				switch(atoi(message)){
					case 0:
					//leave room
						gameState = menuState;
						removeClientFromRoom(roomID);
						deleteThisRoom(roomID);
					break;
					case 3:
					// start game
						if (getClientCount(roomID) == 2){
							// success
							if( sendSuccess(socket, 1, "02", ID) == false ) goto disconnect;
							gameState = player1State;
							setupGame(roomID);
							makeRoomStartGame(roomID);
						}
						else{
							//can't start game
							if( sendSuccess(socket, 0, "06", ID) == false) goto disconnect;
						}
					break;
				}
			}
			break;

			case roomGuestState:
				if(getClientCount(roomID) == 1)
				{//become room owner
					if(sendSuccess(socket, 1, "04", ID) == false) goto disconnect;
					gameState = roomOwnerState;
				}
				if(checkRoomGameStarted(roomID) == true){
					// started game
					if(sendSuccess(socket, 1, "03", ID) == false) goto disconnect;
					gameState = player2State;
				}
			break;
			case player1State:
			firstPlay = false;
				getPlayer1Cards(roomID, message);
				//dealing cards
				if( sendSuccess(socket, 2, message, ID) == false ) goto disconnect;
				if( recvSuccess(socket, &messageCode, message, ID) == false ) goto disconnect;
					if(atoi(message) == 1) {
						firstPlay = true;
						player1GoesFirst(roomID);
					}

					//playgame
					while(1){
						if(checkPlayer1Turn(roomID, &messageCode, message) == true){
							if(firstPlay == false){
							//sending cards player or pass turn info
							if(sendSuccess(socket, messageCode, message, ID) == false) goto disconnect;
							//opponent win or not
							if(ifPlayer2Win(roomID) == true)
								{
									if(sendSuccess(socket, 1, "00", ID) == false) goto disconnect;
									scoreClient(mysqlPtr, username, -1);
									//reset game
									break;
								}
							else if(sendSuccess(socket, 1, "07", ID) == false) goto disconnect;

							if(ifPlayer2Quit(roomID) == true){
								//opponent quit room
								gameState = menuState;
								scoreClient(mysqlPtr, username, 1);
								if(sendSuccess(socket, 1, "06", ID) == false) goto disconnect;
								break;
							}
							else if(sendSuccess(socket, 1, "08", ID) == false) goto disconnect;
							}
							firstPlay = false;
							if(recvSuccess(socket, &messageCode, message, ID) == false) goto disconnect;
							player1Play(roomID, messageCode, message);

							//recv win message or not
							if(recvSuccess(socket, &messageCode, message, ID) == false) goto disconnect;
							if(strcmp(message, "05") == 0) {
								player1Win(roomID);
								player1EndTurn(roomID);
								break;
							}
							player1EndTurn(roomID);
						}
					}
			break;

			case player2State:
			firstPlay = false;
				getPlayer2Cards(roomID, message);
				//dealing cards
				if( sendSuccess(socket, 2, message, ID) == false ) goto disconnect;
				if( recvSuccess(socket, &messageCode, message, ID) == false ) goto disconnect;
					if(atoi(message) == 1) {
						firstPlay = true;
						player2GoesFirst(roomID);
					}
					//play game
					while(1){
						if(checkPlayer2Turn(roomID, &messageCode, message) == true){
							if(firstPlay == false){
							//sending cards player or pass turn info, quit info
							if(sendSuccess(socket, messageCode, message, ID) == false) goto disconnect;
							//opponent win or not
							if(ifPlayer1Win(roomID) == true)
								{
									if(sendSuccess(socket, 1, "00", ID) == false) goto disconnect;
									scoreClient(mysqlPtr, username, -1);
									//reset game
									break;
								}
							else if(sendSuccess(socket, 1, "07", ID) == false) goto disconnect;
							
							if(ifPlayer1Quit(roomID) == true){
								//opponent quit room
								gameState = menuState;
								scoreClient(mysqlPtr, username, 1);
								if(sendSuccess(socket, 1, "06", ID) == false) goto disconnect;
								break;
							}
							else if(sendSuccess(socket, 1, "08", ID) == false) goto disconnect;
							}
							firstPlay = false;
							if(recvSuccess(socket, &messageCode, message, ID) == false) goto disconnect;
							player2Play(roomID, messageCode, message);

							//recv win message or not
							if(recvSuccess(socket, &messageCode, message, ID) == false) goto disconnect;
							if(strcmp(message, "05") == 0) {
								player2Win(roomID);
								player2EndTurn(roomID);
								break;
							}
							player2EndTurn(roomID);
						}
					}
			break;

			default:
			break;
		}
		
	}
	disconnect: 
	if (username[0] != '\0') logoutClient(mysqlPtr, username);
	switch (gameState){
		case roomOwnerState:
		case roomGuestState:
			removeClientFromRoom(roomID);
			deleteThisRoom(roomID);
		break;
		case player1State:
			removeClientFromRoom(roomID);
			deleteThisRoom(roomID);
			player1Quit(roomID);
			scoreClient(mysqlPtr, username, -1);
			player1EndTurn(roomID);
		break;
		case player2State:
			removeClientFromRoom(roomID);
			deleteThisRoom(roomID);
			player2Quit(roomID);
			scoreClient(mysqlPtr, username, -1);
			player2EndTurn(roomID);
		break;
	}
	puts("disconnected");
	return NULL;
}

int customRecv(int socket, int *messageCode, char *buff){
    bzero(buff, BUFF_SIZE);
    char length[6] = "";
	char code[3] = "";
    int i = recv(socket, length, 5, 0);
    if(i <= 0) return 0;
	int len = atoi(length);
	i = recv(socket, code, 2, 0);
    if(i <= 0) return 0;
	*messageCode = atoi(code);
    i = recv(socket, buff, len, 0);
    if(i <= 0) return 0;
	printf("receving %s//%s//%s\n", length, code, buff);
    return i;
}
int customSend(int socket, int messageCode, char *buff){
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
	char code[3];
    char temp[6];
    sprintf(temp, "%d", len);
	if(messageCode < 10) sprintf(code, "0%d", messageCode);
	else sprintf(code, "%d", messageCode);
    sprintf(length, "%s%s", zeroString, temp);
    if(send(socket, length, 5, 0) <= 0) return 0;
	if(send(socket, code, 2, 0) <= 0) return 0;
    if(send(socket, buff, len, 0) <= 0) return 0;
	printf("sending %s//%s//%s\n", length, code, buff);
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

bool sendSuccess(int client_socket, int messageCode, char *message, unsigned int threadID){
	if(customSend(client_socket, messageCode, message) == 0){
		closeSocketAndDeleteThread(client_socket, threadID);
		return false;
	}
	return true;
}
bool recvSuccess(int client_socket, int *messageCode, char *message, unsigned int threadID){
	if(customRecv(client_socket, messageCode, message) == 0){
		closeSocketAndDeleteThread(client_socket, threadID);
		return false;
	}
	return true;
}