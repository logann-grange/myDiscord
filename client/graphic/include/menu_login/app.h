#ifndef APP_H
#define APP_H

#include <gtk/gtk.h>
#include <string.h>

typedef enum {
    ROLE_UTILISATEUR,
    ROLE_MODERATEUR,
    ROLE_ADMINISTRATEUR
} UserRole;

typedef struct {
    GtkWidget *window;
    GtkWidget *stack;
    GtkApplication *app;

    // Login
    GtkWidget *entry_login_pseudo;
    GtkWidget *entry_login_password;

    // Register
    GtkWidget *entry_reg_nom;
    GtkWidget *entry_reg_prenom;
    GtkWidget *entry_reg_pseudo;
    GtkWidget *entry_reg_email;
    GtkWidget *entry_reg_password;

    // Chat
    GtkWidget *main_box;
    GtkWidget *sidebar;
    GtkWidget *chat_area;
    GtkWidget *chan_name_label;
    GtkWidget *messages_box;
    GtkWidget *input_entry;
    GtkWidget *panel_popover;
    GtkWidget *username_label;
    char current_channel[64];
    UserRole role;
    char pseudo[64];
    GtkWidget *active_channel_btn;
    GtkWidget *channels_box;

    // Settings
    GtkWidget *entry_settings_pseudo;
} AppWidgets;

void show_main_window(GtkApplication *app);

#endif