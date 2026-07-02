#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "bddManager.h"

typedef struct {
    int id;
    int idUser;
    int idChannel;
    char *text;
    time_t date;
    char *reaction;
    char *status;
} Message;

char *createMessage(int id, int id_user, int idChannel, time_t date,char *text, char *reaction, char *status);

void writeMessage(Message *message, char c);

void editMessage(Message *message, char *new_message);

void addReaction(Message *message, char *reaction);

int deleteMessageById(int id);





