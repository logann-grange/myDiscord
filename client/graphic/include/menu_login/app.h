#ifndef APP_H
#define APP_H

#include <gtk/gtk.h>

typedef struct {
    GtkWidget *window;
    GtkWidget *stack;
    GtkApplication *app;
    GtkWidget *entry_login_pseudo;
    GtkWidget *entry_login_password;
    GtkWidget *entry_reg_nom;
    GtkWidget *entry_reg_prenom;
    GtkWidget *entry_reg_pseudo;
    GtkWidget *entry_reg_email;
    GtkWidget *entry_reg_password;
} AppWidgets;

void show_main_window(GtkApplication *app);

#endif