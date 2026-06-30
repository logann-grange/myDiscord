#include "../../include/moderateur/mod_panel.h"

static void on_mod_data_free(gpointer data, GClosure *closure) {
    ModPanelData *mpd = (ModPanelData *)data;
    g_free(mpd);
}

static void on_kick_clicked(GtkButton *btn, gpointer data) {
    ModPanelData *d = (ModPanelData *)data;
    const char *pseudo = gtk_entry_get_text(GTK_ENTRY(d->entry_pseudo));

    const char *message = NULL;
    switch (kick_user(pseudo, d->w->role, ROLE_UTILISATEUR)) {
        case MOD_OK:
            g_print("Kick effectué : %s\n", pseudo);
            return;
        case MOD_ERREUR_PERMISSION:
            message = "Permission refusée."; break;
        case MOD_ERREUR_CIBLE_INVALIDE:
            message = "Pseudo invalide."; break;
    }

    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(d->w->window),
        GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "%s", message);
    gtk_widget_show_all(dialog);
    g_signal_connect(dialog, "response", G_CALLBACK(gtk_widget_destroy), NULL);
}

static void on_timeout_clicked(GtkButton *btn, gpointer data) {
    ModPanelData *d = (ModPanelData *)data;
    const char *pseudo = gtk_entry_get_text(GTK_ENTRY(d->entry_pseudo));
    const char *duree_str = gtk_entry_get_text(GTK_ENTRY(d->entry_timeout));
    int duree = atoi(duree_str);

    const char *message = NULL;
    switch (timeout_user(pseudo, duree, d->w->role, ROLE_UTILISATEUR)) {
        case MOD_OK:
            g_print("Timeout effectué : %s pour %d secondes\n", pseudo, duree);
            return;
        case MOD_ERREUR_PERMISSION:
            message = "Permission refusée."; break;
        case MOD_ERREUR_CIBLE_INVALIDE:
            message = "Pseudo invalide."; break;
    }

    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(d->w->window),
        GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "%s", message);
    gtk_widget_show_all(dialog);
    g_signal_connect(dialog, "response", G_CALLBACK(gtk_widget_destroy), NULL);
}

static void on_delete_msg_clicked(GtkButton *btn, gpointer data) {
    ModPanelData *d = (ModPanelData *)data;
    // TODO: récupérer l'id du message sélectionné
    const char *message = NULL;
    switch (delete_message(0, d->w->role, ROLE_UTILISATEUR)) {
        case MOD_OK:
            g_print("Message supprimé\n");
            return;
        case MOD_ERREUR_PERMISSION:
            message = "Permission refusée."; break;
        case MOD_ERREUR_CIBLE_INVALIDE:
            message = "Message invalide."; break;
    }

    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(d->w->window),
        GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "%s", message);
    gtk_widget_show_all(dialog);
    g_signal_connect(dialog, "response", G_CALLBACK(gtk_widget_destroy), NULL);
}

GtkWidget *build_mod_panel(ChatWidgets *w) {
    ModPanelData *d = g_malloc(sizeof(ModPanelData));
    d->w = w;

    GtkWidget *panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_set_name(panel, "mod-panel");
    gtk_widget_set_margin_start(panel, 8);
    gtk_widget_set_margin_end(panel, 8);
    gtk_widget_set_margin_top(panel, 8);
    gtk_widget_set_margin_bottom(panel, 8);

    // Titre
    GtkWidget *title = gtk_label_new("MODÉRATION");
    gtk_widget_set_name(title, "panel-title");
    gtk_widget_set_halign(title, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(panel), title, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(panel),
        gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 4);

    // Champ pseudo cible
    GtkWidget *lbl_pseudo = gtk_label_new("Pseudo de l'utilisateur");
    gtk_widget_set_halign(lbl_pseudo, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(panel), lbl_pseudo, FALSE, FALSE, 0);

    d->entry_pseudo = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(d->entry_pseudo), "pseudo...");
    gtk_box_pack_start(GTK_BOX(panel), d->entry_pseudo, FALSE, FALSE, 0);

    // Bouton kick
    GtkWidget *btn_kick = gtk_button_new_with_label(" Kick");
    gtk_widget_set_name(btn_kick, "mod-btn-kick");
    g_signal_connect_data(btn_kick, "clicked",
        G_CALLBACK(on_kick_clicked), d, on_mod_data_free, 0);
    gtk_box_pack_start(GTK_BOX(panel), btn_kick, FALSE, FALSE, 0);

    // Timeout
    GtkWidget *lbl_timeout = gtk_label_new("Durée timeout (secondes)");
    gtk_widget_set_halign(lbl_timeout, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(panel), lbl_timeout, FALSE, FALSE, 0);

    d->entry_timeout = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(d->entry_timeout), "60");
    gtk_box_pack_start(GTK_BOX(panel), d->entry_timeout, FALSE, FALSE, 0);

    GtkWidget *btn_timeout = gtk_button_new_with_label(" Timeout");
    gtk_widget_set_name(btn_timeout, "mod-btn-timeout");
    g_signal_connect_data(btn_timeout, "clicked",
        G_CALLBACK(on_timeout_clicked), d, on_mod_data_free, 0);
    gtk_box_pack_start(GTK_BOX(panel), btn_timeout, FALSE, FALSE, 0);

    // Supprimer message
    GtkWidget *btn_delete = gtk_button_new_with_label(" Supprimer message");
    gtk_widget_set_name(btn_delete, "mod-btn-delete");
    g_signal_connect_data(btn_delete, "clicked",
        G_CALLBACK(on_delete_msg_clicked), d, on_mod_data_free, 0);
    gtk_box_pack_start(GTK_BOX(panel), btn_delete, FALSE, FALSE, 0);

    return panel;
}


