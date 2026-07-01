#include "../../include/main_chat/settings.h"
#include <string.h>

static void on_save_pseudo_clicked(GtkButton *btn, gpointer data) {
    AppWidgets *w = (AppWidgets *)data;
    const char *new_pseudo = gtk_entry_get_text(GTK_ENTRY(w->entry_settings_pseudo));

    const char *message = NULL;

    switch (validate_pseudo(new_pseudo)) {
        case SETTINGS_PSEUDO_VIDE:
            message = "Le pseudo ne peut pas être vide."; break;
        case SETTINGS_PSEUDO_TROP_COURT:
            message = "Le pseudo doit contenir au moins 3 caractères."; break;
        case SETTINGS_OK:
            strncpy(w->pseudo, new_pseudo, sizeof(w->pseudo) - 1);
            gtk_label_set_text(GTK_LABEL(w->username_label), w->pseudo);
            network_send_update_pseudo(w->pseudo);
            gtk_stack_set_visible_child_name(GTK_STACK(w->stack), "chat");
            return;
    }

    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(w->window),
        GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "%s", message);
    gtk_widget_show_all(dialog);
    g_signal_connect(dialog, "response", G_CALLBACK(gtk_widget_destroy), NULL);
}

static void on_back_to_chat_clicked(GtkButton *btn, gpointer data) {
    AppWidgets *w = (AppWidgets *)data;
    gtk_stack_set_visible_child_name(GTK_STACK(w->stack), "chat");
}

GtkWidget *build_settings_page(AppWidgets *w) {
    GtkWidget *outer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_valign(outer, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(outer, GTK_ALIGN_CENTER);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_widget_set_size_request(box, 400, -1);
    gtk_widget_set_margin_top(box, 48);
    gtk_widget_set_margin_bottom(box, 48);
    gtk_widget_set_margin_start(box, 48);
    gtk_widget_set_margin_end(box, 48);
    gtk_container_add(GTK_CONTAINER(outer), box);

    GtkWidget *title = gtk_label_new("Paramètres");
    gtk_widget_set_name(title, "title");
    gtk_box_pack_start(GTK_BOX(box), title, FALSE, FALSE, 0);

    GtkWidget *subtitle = gtk_label_new("Modifie ton profil");
    gtk_widget_set_name(subtitle, "subtitle");
    gtk_box_pack_start(GTK_BOX(box), subtitle, FALSE, FALSE, 8);

    GtkWidget *lbl_pseudo = gtk_label_new("NOUVEAU PSEUDO");
    gtk_widget_set_halign(lbl_pseudo, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(box), lbl_pseudo, FALSE, FALSE, 0);

    w->entry_settings_pseudo = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(w->entry_settings_pseudo), "Nouveau pseudo");
    gtk_box_pack_start(GTK_BOX(box), w->entry_settings_pseudo, FALSE, FALSE, 0);

    GtkWidget *btn_save = gtk_button_new_with_label("Enregistrer");
    gtk_widget_set_name(btn_save, "suggested-action");
    g_signal_connect(btn_save, "clicked", G_CALLBACK(on_save_pseudo_clicked), w);
    gtk_box_pack_start(GTK_BOX(box), btn_save, FALSE, FALSE, 8);

    GtkWidget *btn_back = gtk_button_new_with_label("← Retour au chat");
    g_signal_connect(btn_back, "clicked", G_CALLBACK(on_back_to_chat_clicked), w);
    gtk_box_pack_start(GTK_BOX(box), btn_back, FALSE, FALSE, 0);

    return outer;
}