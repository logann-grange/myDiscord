#ifndef MOD_PANEL_H
#define MOD_PANEL_H

#include <string.h>
#include <stdio.h>
#include "../main_chat/chat.h"
#include "../../../logic/include/moderateur/mod_logic.h"

typedef struct {
    ChatWidgets *w;
    GtkWidget *entry_pseudo;
    GtkWidget *entry_timeout;
} ModPanelData;

GtkWidget *build_mod_panel(ChatWidgets *w);

#endif