
#ifndef _threads_h_
#define _threads_h_
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include "clients.h"
typedef struct threadList{
	int client_socket;
	pthread_t thread;
	unsigned int ID;
	pthread_mutex_t client_lock;
	struct threadList *next;
}threadList;

threadList *firstThreadSlot, *lastThreadSlot;
pthread_mutex_t lock1, lock2;
unsigned int threadID;
threadList *createThreadSlot(int client_socket);
void insertThisThread(threadList *new);
void deleteThisThread(unsigned int ID);
void deleteAllThreads();
void createThreadClient(int client_socket, bool *threadCreationSuccess);
void closeSocketAndDeleteThread(int client_socket, unsigned int ID);
#endif