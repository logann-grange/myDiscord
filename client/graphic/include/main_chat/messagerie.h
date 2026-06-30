#ifndef MESSAGERIE_H
#define MESSAGERIE_H

#include "../menu_login/app.h"

GtkWidget *build_messagerie(AppWidgets *w);
GtkWidget *build_message(const char *auteur, const char *heure, const char *texte, const char avatar_lettre);

#endif