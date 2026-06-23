#include "../include/message.h"

char *createMessage(int id, int id_user, time_t date,char *text, char *reaction, char *status)
{
    Message *msg = malloc(sizeof(Message));
    msg->id = id;
    msg->idUser = id_user;
    msg->date = date;
    msg->reaction = reaction;
    msg->status = status;
    msg->text = text;

    // ajouter la requette sql

    return msg;
}

void writeMessage(Message *message, char c) // ajoute un caractere au message
{
    int len = (*message->text == NULL) ? 0 : strlen(*message->text);
    char *new_message = realloc(*message->text, len + 2); // +1 pour c, +1 pour '\0'
    if (new_message == NULL) {
        // gère l'erreur d'allocation
        return;
    }
    new_message[len] = c;
    new_message[len + 1] = '\0';
    message->text = new_message;
    message->status = "en cours";
}

void editMessage(Message *message, char *new_message)
{
    int new_len = strlen(*new_message);
    message->text = *new_message;
    message->status = "modifié";

    // ajouter la fonction de la requette sql
}

void deleteMessage(Message *message)
{
    message->status = "supprimer";

    // ajouter la requette sql update ou delete (selon si on veux suppr dans la bdd)

    free(message);
}

void addReaction(Message *message, char *reaction)
{
    // message->reaction de forme : reaction1;reaction2;reaction3;
    int len = (*message->reaction == NULL) ? 0 : strlen(*message->text);
    char *new_reaction = realloc(*message->text, len + strlen(*reaction) + 1); // +1 pour '\0'
    if (new_reaction == NULL) {
        return;
    }
    for (int i=len;i<=strlen(*reaction);i++) {
        new_reaction[i] = reaction[i-len];
    }

    new_reaction[len + strlen(*reaction)] = '\0';
    message->reaction = new_reaction;
    // ajouter la raquette sql update
}