#include "deque.h"
void addRear(char **strList, char *str){
    for(int i = 3; i >= 0; --i){
        if(strcmp(strList[i], "") == 0){
            strcpy(strList[i], str);
            break;
        }
    }
}