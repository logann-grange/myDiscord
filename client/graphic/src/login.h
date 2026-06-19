#ifndef LOGIN_H
#define LOGIN_H

#include <gtk/gtk.h>


typedef struct 
{
    GtkWidget *window;
    GtkWidget *entry_pseudo;
    GtkWidget *entry_password;
    GtkApplication *app;
} LoginWidgets;

void on_login_clicked(GtkButton *btn, gpointer data);

void show_login_window(GtkApplication *app);




#endif 