#ifndef _database_h_
#define _database_h_
#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define BUFF_SIZE 1024
extern pthread_mutex_t dbLock;
extern MYSQL *mysqlPtr;
void finish_with_error(MYSQL *databasePtr);
int createNewClient(MYSQL *mysqlPtr, char *username, char *password, bool loginStatus);
int loginClient(MYSQL *mysqlPtr, char *username, char *password);
int logoutClient(MYSQL *mysqlPtr, char *username);
int scoreClient(MYSQL *mysqlPtr, char *username, int points);
#endif