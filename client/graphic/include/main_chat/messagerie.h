#ifndef MESSAGERIE_H
#define MESSAGERIE_H

#include "chat.h"

GtkWidget *build_messagerie(ChatWidgets *w);
GtkWidget *build_message(const char *auteur, const char *heure, const char *texte, const char avatar_lettre);

#endif