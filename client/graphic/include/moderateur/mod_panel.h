#ifndef MOD_PANEL_H
#define MOD_PANEL_H

#include <string.h>
#include <stdio.h>
#include "../menu_login/app.h"
#include "../../../logic/include/moderateur/mod_logic.h"

typedef struct {
    AppWidgets *w;
    GtkWidget *entry_pseudo;
    GtkWidget *entry_timeout;
} ModPanelData;

GtkWidget *build_mod_panel(AppWidgets *w);

#endif