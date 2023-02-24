#include "database.h"

pthread_mutex_t dbLock;
MYSQL *mysqlPtr = NULL;
void finish_with_error(MYSQL *mysqlPtr)
{
  fprintf(stderr, "%s\n", mysql_error(mysqlPtr));
}

int createNewClient(MYSQL *mysqlPtr, char *username, char *password, bool loginStatus)
{
  pthread_mutex_lock(&dbLock);
  char query[BUFF_SIZE];
  sprintf(query,"INSERT INTO info VALUES('%s','%s', %d, %s)", username, password, 0, loginStatus?"true":"false");
  if (mysql_query(mysqlPtr, query)) {
      finish_with_error(mysqlPtr);
      pthread_mutex_unlock(&dbLock);
      return 1; //username existed
  }
  pthread_mutex_unlock(&dbLock);
  return 0;
}
int loginClient(MYSQL *mysqlPtr, char *username, char *password){
  pthread_mutex_lock(&dbLock);
  char query[BUFF_SIZE];
  sprintf(query,"SELECT password, loginStatus FROM info WHERE username = \'%s\'", username);
  if (mysql_query(mysqlPtr, query)) {
      finish_with_error(mysqlPtr);
      pthread_mutex_unlock(&dbLock);
      return 1; //username doesn't exist
  }

  MYSQL_RES *result = mysql_store_result(mysqlPtr);
  int numFields = mysql_num_fields(result);
  char dbPassword[BUFF_SIZE];
  MYSQL_ROW row = mysql_fetch_row(result);
  strcpy(dbPassword, row[0]);
  int loginStatus = atoi(row[1]);
  
  if (strcmp(dbPassword, password) != 0)
  {
    pthread_mutex_unlock(&dbLock);
    return 2; //wrong password
  }

  if(loginStatus == 1){
    pthread_mutex_unlock(&dbLock);
    return 3; //logged in somewhere else
  }

  //login success
  sprintf(query, "UPDATE info SET loginStatus = 1 WHERE username = \'%s\'", username);
   if (mysql_query(mysqlPtr, query)) {
      finish_with_error(mysqlPtr);
      pthread_mutex_unlock(&dbLock);
      return 4; //database error
  }

  
  pthread_mutex_unlock(&dbLock);
  return 0;
}

int logoutClient(MYSQL *mysqlPtr, char *username){
  pthread_mutex_lock(&dbLock);
  char query[BUFF_SIZE];
  sprintf(query,"UPDATE info SET loginStatus=0 WHERE username=\'%s\'", username);
  if (mysql_query(mysqlPtr, query)){
      finish_with_error(mysqlPtr);
      pthread_mutex_unlock(&dbLock);
      return 1; //database error
  }
  pthread_mutex_unlock(&dbLock);
  return 0;
}

int scoreClient(MYSQL *mysqlPtr, char *username, int points){
  pthread_mutex_lock(&dbLock);
  char query[BUFF_SIZE];
  if( points == -1) sprintf(query,"UPDATE info SET score = score - 1 WHERE username = \'%s\'", username);
  else sprintf(query,"UPDATE info SET score = score + %d WHERE username = \'%s\'", points, username);
  if (mysql_query(mysqlPtr, query)){
      finish_with_error(mysqlPtr);
      pthread_mutex_unlock(&dbLock);
      return 1; //database error
  }
  pthread_mutex_unlock(&dbLock);
  return 0;
}