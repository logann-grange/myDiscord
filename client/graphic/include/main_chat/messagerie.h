#ifndef MESSAGERIE_H
#define MESSAGERIE_H

#include "../menu_login/app.h"
#include "../../../logic/include/menu_login/network.h"

GtkWidget *build_messagerie(AppWidgets *w);
GtkWidget *build_message(int message_id, const char *auteur, const char *heure, const char *texte, const char avatar_lettre);
void apply_message_deleted(AppWidgets *w, const char *channel, int message_id);

typedef struct {
    AppWidgets *w;
    int message_id;
    char auteur[64];
    char channel[64];
    char texte[512];
    char date[32];
} IncomingMessageData;

gboolean display_incoming_message(gpointer data);

#endif