#include "../../include/moderateur/admin_panel.h"
#include "moderateur/mod_logic.h"
#include "moderateur/admin_logic.h"
#include <string.h>
#include <stdio.h>

typedef struct {
    ChatWidgets *w;
    GtkWidget *entry_canal;
    GtkWidget *entry_pseudo;
    GtkWidget *combo_role;
} AdminPanelData;

static void on_admin_data_free(gpointer data, GClosure *closure) {
    g_free(data);
}

static void on_create_channel_clicked(GtkButton *btn, gpointer data) {
    AdminPanelData *d = (AdminPanelData *)data;
    const char *canal = gtk_entry_get_text(GTK_ENTRY(d->entry_canal));

    const char *message = NULL;
    switch (create_channel(canal, d->w->role)) {
        case ADMIN_OK:
            g_print("Canal créé : %s\n", canal);
            return;
        case ADMIN_ERREUR_PERMISSION:
            message = "Permission refusée."; break;
        case ADMIN_ERREUR_CIBLE_INVALIDE:
            message = "Nom de canal invalide."; break;
    }

    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(d->w->window),
        GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "%s", message);
    gtk_widget_show_all(dialog);
    g_signal_connect(dialog, "response", G_CALLBACK(gtk_widget_destroy), NULL);
}

static void on_delete_channel_clicked(GtkButton *btn, gpointer data) {
    AdminPanelData *d = (AdminPanelData *)data;
    const char *canal = gtk_entry_get_text(GTK_ENTRY(d->entry_canal));

    const char *message = NULL;
    switch (delete_channel(canal, d->w->role)) {
        case ADMIN_OK:
            g_print("Canal supprimé : %s\n", canal);
            return;
        case ADMIN_ERREUR_PERMISSION:
            message = "Permission refusée."; break;
        case ADMIN_ERREUR_CIBLE_INVALIDE:
            message = "Nom de canal invalide."; break;
    }

    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(d->w->window),
        GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "%s", message);
    gtk_widget_show_all(dialog);
    g_signal_connect(dialog, "response", G_CALLBACK(gtk_widget_destroy), NULL);
}

static void on_set_role_clicked(GtkButton *btn, gpointer data) {
    AdminPanelData *d = (AdminPanelData *)data;
    const char *pseudo = gtk_entry_get_text(GTK_ENTRY(d->entry_pseudo));
    int role_idx = gtk_combo_box_get_active(GTK_COMBO_BOX(d->combo_role));
    UserRole nouveau_role = (UserRole)role_idx;

    const char *message = NULL;
    switch (set_user_role(pseudo, nouveau_role, d->w->role)) {
        case ADMIN_OK:
            g_print("Rôle de %s changé\n", pseudo);
            return;
        case ADMIN_ERREUR_PERMISSION:
            message = "Permission refusée."; break;
        case ADMIN_ERREUR_CIBLE_INVALIDE:
            message = "Pseudo invalide."; break;
    }

    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(d->w->window),
        GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "%s", message);
    gtk_widget_show_all(dialog);
    g_signal_connect(dialog, "response", G_CALLBACK(gtk_widget_destroy), NULL);
}

GtkWidget *build_admin_panel(ChatWidgets *w) {
    AdminPanelData *d = g_malloc(sizeof(AdminPanelData));
    d->w = w;

    GtkWidget *panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_set_name(panel, "admin-panel");
    gtk_widget_set_margin_start(panel, 8);
    gtk_widget_set_margin_end(panel, 8);
    gtk_widget_set_margin_top(panel, 8);
    gtk_widget_set_margin_bottom(panel, 8);

    // Titre
    GtkWidget *title = gtk_label_new("ADMINISTRATION");
    gtk_widget_set_name(title, "panel-title");
    gtk_widget_set_halign(title, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(panel), title, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(panel),
        gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 4);

    // ── Section canaux ──
    GtkWidget *lbl_canaux = gtk_label_new("CANAUX");
    gtk_widget_set_name(lbl_canaux, "panel-section");
    gtk_widget_set_halign(lbl_canaux, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(panel), lbl_canaux, FALSE, FALSE, 0);

    d->entry_canal = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(d->entry_canal), "nom-du-canal");
    gtk_box_pack_start(GTK_BOX(panel), d->entry_canal, FALSE, FALSE, 0);

    GtkWidget *btn_create = gtk_button_new_with_label("➕ Créer canal");
    gtk_widget_set_name(btn_create, "admin-btn-create");
    g_signal_connect_data(btn_create, "clicked",
        G_CALLBACK(on_create_channel_clicked), d, on_admin_data_free, 0);
    gtk_box_pack_start(GTK_BOX(panel), btn_create, FALSE, FALSE, 0);

    GtkWidget *btn_delete_canal = gtk_button_new_with_label("🗑 Supprimer canal");
    gtk_widget_set_name(btn_delete_canal, "admin-btn-delete");
    g_signal_connect_data(btn_delete_canal, "clicked",
        G_CALLBACK(on_delete_channel_clicked), d, on_admin_data_free, 0);
    gtk_box_pack_start(GTK_BOX(panel), btn_delete_canal, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(panel),
        gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 4);

    // ── Section rôles ──
    GtkWidget *lbl_roles = gtk_label_new("RÔLES");
    gtk_widget_set_name(lbl_roles, "panel-section");
    gtk_widget_set_halign(lbl_roles, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(panel), lbl_roles, FALSE, FALSE, 0);

    d->entry_pseudo = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(d->entry_pseudo), "pseudo...");
    gtk_box_pack_start(GTK_BOX(panel), d->entry_pseudo, FALSE, FALSE, 0);

    d->combo_role = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(d->combo_role), "Utilisateur");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(d->combo_role), "Modérateur");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(d->combo_role), "Administrateur");
    gtk_combo_box_set_active(GTK_COMBO_BOX(d->combo_role), 0);
    gtk_box_pack_start(GTK_BOX(panel), d->combo_role, FALSE, FALSE, 0);

    GtkWidget *btn_set_role = gtk_button_new_with_label("👑 Changer rôle");
    gtk_widget_set_name(btn_set_role, "admin-btn-role");
    g_signal_connect_data(btn_set_role, "clicked",
        G_CALLBACK(on_set_role_clicked), d, on_admin_data_free, 0);
    gtk_box_pack_start(GTK_BOX(panel), btn_set_role, FALSE, FALSE, 0);

    return panel;
}