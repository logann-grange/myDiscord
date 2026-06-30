#ifndef CHAT_H
#define CHAT_H

#include <gtk/gtk.h>
#include <string.h>

typedef enum{
ROLE_UTILISATEUR,
ROLE_MODERATEUR,
ROLE_ADMINISTRATEUR

} UserRole;

typedef struct {
    GtkWidget *window;
    GtkWidget *main_box;
    GtkWidget *sidebar;
    GtkWidget *chat_area;
    GtkWidget *chan_name_label;    
    GtkWidget *messages_box;       
    GtkWidget *input_entry;
    GtkWidget *panel_popover;         
    char current_channel[64];
    UserRole role;
    char pseudo[64];
} ChatWidgets;

void show_chat_window(GtkApplication *app);

#endif