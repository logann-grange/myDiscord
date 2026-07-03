#ifndef SERVER_INCLUDE_MESSAGE_H
#define SERVER_INCLUDE_MESSAGE_H

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "bddManager.h"
#include <time.h> 

typedef struct {
    int id;
    int idUser;
    int idChannel;
    char *text;
    time_t date;
    char *reaction;
    char *status;
} Message;

/* retourne un pointeur vers un Message alloué */
Message *createMessage(int id, int id_user, int idChannel, time_t date, char *text, char *reaction, char *status);

void writeMessage(Message *message, char c);

void editMessage(Message *message, char *new_message);

void addReaction(Message *message, char *reaction);

int deleteMessageById(int id);

/* insertion d'un message en base — retourne l'id ou -1 */
int insertMessage(Message *message);

#endif /* SERVER_INCLUDE_MESSAGE_H */





