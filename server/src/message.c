#include "../include/message.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// FIX: le type de retour était "char *" alors que la fonction construit et
// retourne un Message*. Cela ne compilait pas (incompatibilité avec message.h,
// qui déclare probablement Message *createMessage(...)).
Message *createMessage(int id, int id_user, int idChannel, time_t date, char *text, char *reaction, char *status)
{
    Message *msg = malloc(sizeof(Message));
    if (!msg) return NULL;

    msg->id = id;
    msg->idUser = id_user;
    msg->idChannel = idChannel;
    msg->date = date;
    msg->reaction = reaction;
    msg->status = status;
    msg->text = text;

    return msg;
}

// FIX: message->text est un char*, pas un char**. Le code original faisait
// *message->text (déréférence invalide) et réaffectait message->text = new_message
// après un realloc(*message->text, ...) sur une variable non initialisée : UB garanti.
void writeMessage(Message *message, char c) // ajoute un caractere au message
{
    int len = (message->text == NULL) ? 0 : strlen(message->text);
    char *new_message = realloc(message->text, len + 2); // +1 pour c, +1 pour '\0'
    if (new_message == NULL) {
        // gère l'erreur d'allocation
        return;
    }
    new_message[len] = c;
    new_message[len + 1] = '\0';
    message->text = new_message;
    message->status = "en cours";
}

int insertMessage(Message *message)
{
    char idUserStr[16], idChannelStr[16], dateStr[32];
    snprintf(idUserStr, sizeof(idUserStr), "%d", message->idUser);
    snprintf(idChannelStr, sizeof(idChannelStr), "%d", message->idChannel);
    snprintf(dateStr, sizeof(dateStr), "%ld", (long)message->date);

    char *fields[] = {"id_user", "date", "text", "status", "id_channel"};
    char *params[] = {idUserStr, dateStr, message->text, message->status, idChannelStr};

    return bddInsert("message", fields, params, 5);
}

// FIX: new_message est un char* (pas char**). L'original faisait
// message->text = *new_message (assignait un simple char à un char*, erreur de
// type) et n'utilisait jamais new_len. On duplique proprement la nouvelle
// chaîne, on libère l'ancienne pour éviter une fuite mémoire, et on persiste
// à la fois le texte ET le statut en une seule requête (l'original oubliait
// de sauvegarder le statut "modifié" en base).
void editMessage(Message *message, char *new_message)
{
    char *copy = strdup(new_message);
    if (copy == NULL) return;

    free(message->text);
    message->text = copy;
    message->status = "modifié";

    char *fields[] = {"text", "status"};
    char *params[] = {message->text, message->status};
    bddUpdate("message", fields, params, 2, message->id);
}

void deleteMessage(Message *message)
{
    message->status = "supprimé";

    char *fields[] = {"status"};
    char *params[] = {message->status};
    bddUpdate("message", fields, params, 1, message->id);

    free(message);
}

// FIX: même bug de typage que writeMessage (déréférencement de char* comme
// char**, et réutilisation erronée de message->text au lieu de message->reaction
// dans le calcul de longueur / realloc). La boucle de copie utilisait aussi
// une condition "i <= strlen(*reaction)" qui lisait un octet hors-limites.
// On ajoute enfin l'appel bddUpdate manquant (le commentaire d'origine
// indiquait "ajouter la requette sql update" mais rien n'était fait).
void addReaction(Message *message, char *reaction)
{
    // message->reaction de forme : reaction1;reaction2;reaction3;
    int len = (message->reaction == NULL) ? 0 : strlen(message->reaction);
    int reaction_len = strlen(reaction);

    char *new_reaction = realloc(message->reaction, len + reaction_len + 1); // +1 pour '\0'
    if (new_reaction == NULL) {
        return;
    }

    for (int i = 0; i < reaction_len; i++) {
        new_reaction[len + i] = reaction[i];
    }
    new_reaction[len + reaction_len] = '\0';

    message->reaction = new_reaction;

    char *fields[] = {"reaction"};
    char *params[] = {message->reaction};
    bddUpdate("message", fields, params, 1, message->id);
}

int deleteMessageById(int id) {
    char idStr[16];
    snprintf(idStr, sizeof(idStr), "%d", id);
    char *fields[] = {"status"};
    char *params[] = {"supprimé"};
    return bddUpdate("message", fields, params, 1, id);
}