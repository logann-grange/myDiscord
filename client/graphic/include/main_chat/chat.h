#ifndef CHAT_H
#define CHAT_H

#include <gtk/gtk.h>

typedef struct {
    GtkWidget *window;
    GtkWidget *main_box;
    GtkWidget *sidebar;
    GtkWidget *chat_area;
} ChatWidgets;

void show_chat_window(GtkApplication *app);

#endif